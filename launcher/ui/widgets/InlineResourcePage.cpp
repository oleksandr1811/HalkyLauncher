// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#include "InlineResourcePage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/mod/ModFolderModel.h"
#include "minecraft/mod/ResourcePackFolderModel.h"
#include "minecraft/mod/ShaderPackFolderModel.h"
#include "tasks/ConcurrentTask.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/ProgressDialog.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

InlineResourcePage::InlineResourcePage(BrowseMode mode, QWidget* parent)
    : QWidget(parent), m_mode(mode)
{
    setObjectName(QStringLiteral("inlineResourcePage"));
    buildLayout();
}

static QString modePageTitle(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Mods:          return QObject::tr("Browse Mods");
        case BrowseMode::ResourcePacks: return QObject::tr("Browse Resource Packs");
        case BrowseMode::ShaderPacks:   return QObject::tr("Browse Shader Packs");
        default:                        return {};
    }
}

void InlineResourcePage::buildLayout()
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Header bar ───────────────────────────────────────────────────────────
    auto* header = new QWidget(this);
    header->setObjectName(QStringLiteral("inlinePageHeader"));
    header->setFixedHeight(52);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 8, 16, 8);
    headerLayout->setSpacing(12);

    auto* titleLbl = new QLabel(modePageTitle(m_mode), header);
    titleLbl->setObjectName(QStringLiteral("inlinePageTitle"));
    headerLayout->addWidget(titleLbl);
    headerLayout->addStretch(1);

    auto* instLbl = new QLabel(tr("Install into:"), header);
    instLbl->setObjectName(QStringLiteral("browsePageLabel"));
    headerLayout->addWidget(instLbl);

    m_instanceCombo = new QComboBox(header);
    m_instanceCombo->setObjectName(QStringLiteral("browseInstanceCombo"));
    m_instanceCombo->setMinimumWidth(200);
    headerLayout->addWidget(m_instanceCombo);

    outer->addWidget(header);

    // Separator
    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("libraryHeaderSep"));
    outer->addWidget(sep);

    // ── Browser area ─────────────────────────────────────────────────────────
    m_browserArea = new QWidget(this);
    m_browserArea->setObjectName(QStringLiteral("inlineBrowserArea"));
    m_browserLayout = new QVBoxLayout(m_browserArea);
    m_browserLayout->setContentsMargins(0, 0, 0, 0);
    m_browserLayout->setSpacing(0);

    m_noInstanceLabel = new QLabel(tr("Select an instance above to start browsing."), m_browserArea);
    m_noInstanceLabel->setObjectName(QStringLiteral("inlineNoInstanceLabel"));
    m_noInstanceLabel->setAlignment(Qt::AlignCenter);
    m_browserLayout->addWidget(m_noInstanceLabel);
    m_browserLayout->addStretch(1);

    outer->addWidget(m_browserArea, 1);

    // ── Populate combo + live updates ────────────────────────────────────────
    updateInstanceCombo();

    auto* instances = APPLICATION->instances();
    connect(instances, &QAbstractItemModel::rowsInserted, this,
            [this](const QModelIndex&, int, int) { updateInstanceCombo(); });
    connect(instances, &QAbstractItemModel::rowsRemoved, this,
            [this](const QModelIndex&, int, int) { updateInstanceCombo(); });
    connect(instances, &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex&, const QModelIndex&, const QList<int>&) { updateInstanceCombo(); });

    connect(m_instanceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InlineResourcePage::onInstanceComboChanged);
}

void InlineResourcePage::updateInstanceCombo()
{
    // Remember current selection
    const QString prevId = m_instanceCombo->currentData().toString();

    const QSignalBlocker blocker(m_instanceCombo);
    m_instanceCombo->clear();

    auto* instances = APPLICATION->instances();
    int selectIdx = -1;
    for (int i = 0; i < instances->count(); ++i) {
        auto* inst = instances->at(i);
        if (!inst) continue;
        m_instanceCombo->addItem(inst->name(), inst->id());
        if (inst->id() == prevId)
            selectIdx = i;
    }

    if (m_instanceCombo->count() == 0) {
        m_instanceCombo->addItem(tr("No instances available"), QString());
        clearBrowser();
        return;
    }

    if (selectIdx >= 0)
        m_instanceCombo->setCurrentIndex(selectIdx);
    else
        m_instanceCombo->setCurrentIndex(0);

    // Trigger browser refresh (index may not have changed, so do it explicitly).
    // Use a deferred call so this never runs during widget construction — creating
    // a ResourceDownloadDialog during buildLayout() crashes in debug builds because
    // PageContainer tries to start Modrinth/Flame requests before Qt is fully set up.
    const int idx = m_instanceCombo->currentIndex();
    QTimer::singleShot(0, this, [this, idx]() { onInstanceComboChanged(idx); });
}

void InlineResourcePage::setCurrentInstanceById(const QString& id)
{
    for (int i = 0; i < m_instanceCombo->count(); ++i) {
        if (m_instanceCombo->itemData(i).toString() == id) {
            m_instanceCombo->setCurrentIndex(i);
            return;
        }
    }
}

void InlineResourcePage::onInstanceComboChanged(int index)
{
    if (index < 0 || m_instanceCombo->count() == 0) {
        clearBrowser();
        return;
    }

    const QString id = m_instanceCombo->itemData(index).toString();
    if (id.isEmpty()) {
        clearBrowser();
        return;
    }

    // Find instance by ID
    auto* instances = APPLICATION->instances();
    for (int i = 0; i < instances->count(); ++i) {
        auto* inst = instances->at(i);
        if (inst && inst->id() == id) {
            showBrowserForInstance(inst);
            return;
        }
    }
    clearBrowser();
}

void InlineResourcePage::showBrowserForInstance(BaseInstance* inst)
{
    auto* mcinst = dynamic_cast<MinecraftInstance*>(inst);
    if (!mcinst) {
        clearBrowser();
        return;
    }

    // Remove and delete the old dialog
    if (m_dlg) {
        m_browserLayout->removeWidget(m_dlg);
        delete m_dlg;
        m_dlg = nullptr;
    }
    if (m_noInstanceLabel) {
        m_noInstanceLabel->setVisible(false);
    }

    // Create the appropriate dialog for this mode
    switch (m_mode) {
        case BrowseMode::Mods:
            m_dlg = new ResourceDownload::ModDownloadDialog(m_browserArea, mcinst->loaderModList(), mcinst);
            break;
        case BrowseMode::ResourcePacks:
            m_dlg = new ResourceDownload::ResourcePackDownloadDialog(m_browserArea, mcinst->resourcePackList(), mcinst);
            break;
        case BrowseMode::ShaderPacks:
            m_dlg = new ResourceDownload::ShaderPackDownloadDialog(m_browserArea, mcinst->shaderPackList(), mcinst);
            break;
        default:
            return;
    }

    // Embed the dialog as a plain widget (no separate window)
    m_dlg->setWindowFlags(Qt::Widget);
    m_dlg->setWindowModality(Qt::NonModal);
    m_dlg->setAttribute(Qt::WA_DeleteOnClose, false);
    m_dlg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_browserLayout->insertWidget(0, m_dlg);
    m_dlg->setVisible(true);

    // Use QueuedConnection so the handler runs after accept()/reject() returns
    connect(m_dlg, &QDialog::finished, this, &InlineResourcePage::onDialogFinished,
            Qt::QueuedConnection);
}

void InlineResourcePage::clearBrowser()
{
    if (m_dlg) {
        m_browserLayout->removeWidget(m_dlg);
        delete m_dlg;
        m_dlg = nullptr;
    }
    if (m_noInstanceLabel)
        m_noInstanceLabel->setVisible(true);
}

void InlineResourcePage::onDialogFinished(int result)
{
    auto* oldDlg = m_dlg;
    m_dlg = nullptr;

    ResourceFolderModel* model = nullptr;
    QList<ResourceDownload::ResourceDownloadDialog::DownloadTaskPtr> tasks;

    if (oldDlg) {
        model = oldDlg->getBaseModel();
        if (result == QDialog::Accepted)
            tasks = oldDlg->getTasks();
        m_browserLayout->removeWidget(oldDlg);
    }

    // Run download tasks (blocking, shows ProgressDialog)
    if (!tasks.isEmpty())
        runDownloadTasks(tasks);

    // Refresh the folder model so the new files appear
    if (model)
        model->update();

    if (oldDlg) {
        oldDlg->deleteLater();
    }

    // Re-create the browser for the same instance
    QTimer::singleShot(0, this, [this]() {
        onInstanceComboChanged(m_instanceCombo->currentIndex());
    });
}

void InlineResourcePage::runDownloadTasks(
    const QList<ResourceDownload::ResourceDownloadDialog::DownloadTaskPtr>& tasks)
{
    if (tasks.isEmpty())
        return;

    auto* concTask = new ConcurrentTask(
        tr("Download Resources"),
        APPLICATION->settings()->get("NumberOfConcurrentDownloads").toInt());

    connect(concTask, &Task::failed, this, [this, concTask](const QString& reason) {
        CustomMessageBox::selectable(this, tr("Error"), reason, QMessageBox::Critical)->show();
        concTask->deleteLater();
    });
    connect(concTask, &Task::aborted, this, [this, concTask]() {
        CustomMessageBox::selectable(this, tr("Aborted"), tr("Download stopped by user."),
                                     QMessageBox::Information)
            ->show();
        concTask->deleteLater();
    });
    connect(concTask, &Task::succeeded, this, [concTask]() { concTask->deleteLater(); });

    for (const auto& task : tasks)
        concTask->addTask(task);

    ProgressDialog pd(this);
    pd.setSkipButton(true, tr("Abort"));
    pd.execWithTask(concTask);
}
