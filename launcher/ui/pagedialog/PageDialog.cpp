/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PageDialog.h"

#include <QDialogButtonBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "Application.h"
#include "settings/SettingsObject.h"

#include "ui/widgets/PageContainer.h"

PageDialog::PageDialog(BasePageProvider* pageProvider, QString defaultId, QWidget* parent) : QDialog(parent)
{
    setObjectName(QStringLiteral("settingsDialog"));
    setWindowTitle(pageProvider->dialogTitle());
    setMinimumSize(860, 580);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ── Title bar ────────────────────────────────────────────────────────────
    auto* titleBar = new QFrame(this);
    titleBar->setObjectName(QStringLiteral("settingsTitleBar"));
    titleBar->setFrameShape(QFrame::NoFrame);
    auto* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(20, 14, 20, 14);
    titleLayout->setSpacing(12);

    auto* titleIcon = new QLabel(titleBar);
    titleIcon->setObjectName(QStringLiteral("settingsTitleIcon"));
    titleIcon->setFixedSize(24, 24);
    titleIcon->setPixmap(QIcon::fromTheme(QStringLiteral("settings")).pixmap(24, 24));
    titleLayout->addWidget(titleIcon);

    auto* titleLabel = new QLabel(pageProvider->dialogTitle(), titleBar);
    titleLabel->setObjectName(QStringLiteral("settingsTitleLabel"));
    titleLayout->addWidget(titleLabel, 1);

    mainLayout->addWidget(titleBar);

    // ── Focus stealer (prevents auto-focus on first input field) ─────────────
    auto* focusStealer = new QPushButton(this);
    mainLayout->addWidget(focusStealer);
    focusStealer->setDefault(true);
    focusStealer->hide();

    // ── Page container ────────────────────────────────────────────────────────
    m_container = new PageContainer(pageProvider, std::move(defaultId), this);
    m_container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_container->layout()->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_container);

    setLayout(mainLayout);

    // ── Buttons ───────────────────────────────────────────────────────────────
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->setContentsMargins(0, 0, 12, 10);
    buttons->setObjectName(QStringLiteral("settingsButtons"));

    auto* okBtn = buttons->button(QDialogButtonBox::Ok);
    okBtn->setText(tr("Apply && Close"));
    okBtn->setObjectName(QStringLiteral("settingsOkBtn"));
    okBtn->setDefault(true);
    okBtn->setAutoDefault(true);

    auto* cancelBtn = buttons->button(QDialogButtonBox::Cancel);
    cancelBtn->setText(tr("Cancel"));
    cancelBtn->setDefault(false);
    cancelBtn->setAutoDefault(false);

    auto* helpBtn = buttons->button(QDialogButtonBox::Help);
    helpBtn->setText(tr("Help"));
    helpBtn->setDefault(false);
    helpBtn->setAutoDefault(false);

    m_container->addButtons(buttons);

    connect(okBtn, &QPushButton::clicked, this, &PageDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &PageDialog::reject);
    connect(helpBtn, &QPushButton::clicked, m_container, &PageContainer::help);

    restoreGeometry(QByteArray::fromBase64(APPLICATION->settings()->get("PagedGeometry").toString().toUtf8()));
}

void PageDialog::accept()
{
    if (handleClose())
        QDialog::accept();
}

void PageDialog::closeEvent(QCloseEvent* event)
{
    if (handleClose())
        QDialog::closeEvent(event);
}

bool PageDialog::handleClose()
{
    qDebug() << "Paged dialog close requested";
    if (!m_container->prepareToClose())
        return false;

    qDebug() << "Paged dialog close approved";
    APPLICATION->settings()->set("PagedGeometry", QString::fromUtf8(saveGeometry().toBase64()));
    qDebug() << "Paged dialog geometry saved";

    emit applied();
    return true;
}
