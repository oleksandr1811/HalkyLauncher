// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#include "BrowsePage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"

// ── Platform descriptor ───────────────────────────────────────────────────────

struct Platform {
    QString id;
    QString name;
    QString desc;
    QString iconName;
};

static QList<Platform> platformsForMode(BrowseMode mode)
{
    const bool hasFlame = APPLICATION->capabilities() & Application::SupportsFlame;

    QList<Platform> list;
    switch (mode) {
        case BrowseMode::Modpacks:
            list << Platform{ "modrinth", "Modrinth",   QObject::tr("Open-source, community-driven modpacks"),   "modrinth" };
            if (hasFlame)
                list << Platform{ "flame", "CurseForge", QObject::tr("Huge library of official CurseForge packs"), "flame" };
            list << Platform{ "ftb",     "FTB",          QObject::tr("Feed the Beast curated packs"),             "ftb_logo" };
            list << Platform{ "atl",     "ATLauncher",   QObject::tr("ATLauncher community packs"),               "atlauncher" };
            list << Platform{ "technic", "Technic",      QObject::tr("Technic Platform modpacks"),                "technic" };
            break;
        case BrowseMode::Mods:
            list << Platform{ "modrinth", "Modrinth",   QObject::tr("Open-source mods from the community"),      "modrinth" };
            if (hasFlame)
                list << Platform{ "flame", "CurseForge", QObject::tr("Official CurseForge mod library"),          "flame" };
            break;
        case BrowseMode::ResourcePacks:
            list << Platform{ "modrinth", "Modrinth",   QObject::tr("Community resource packs"),                  "modrinth" };
            if (hasFlame)
                list << Platform{ "flame", "CurseForge", QObject::tr("CurseForge resource packs"),                "flame" };
            break;
        case BrowseMode::ShaderPacks:
            list << Platform{ "modrinth", "Modrinth",   QObject::tr("Community-made shader packs"),               "modrinth" };
            if (hasFlame)
                list << Platform{ "flame", "CurseForge", QObject::tr("CurseForge shader packs"),                  "flame" };
            break;
    }
    return list;
}

// ── BrowsePage ────────────────────────────────────────────────────────────────

BrowsePage::BrowsePage(BrowseMode mode, QWidget* parent) : QWidget(parent), m_mode(mode)
{
    setObjectName(QStringLiteral("browsePage"));
    buildLayout();
}

static QString modeTitle(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:      return QObject::tr("Browse Modpacks");
        case BrowseMode::Mods:          return QObject::tr("Browse Mods");
        case BrowseMode::ResourcePacks: return QObject::tr("Browse Resource Packs");
        case BrowseMode::ShaderPacks:   return QObject::tr("Browse Shader Packs");
    }
    return {};
}

static QString modeDesc(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:
            return QObject::tr("Install modpacks from Modrinth, CurseForge, FTB, ATLauncher and more.");
        case BrowseMode::Mods:
            return QObject::tr("Search and install mods for a selected instance.");
        case BrowseMode::ResourcePacks:
            return QObject::tr("Search and install resource packs for a selected instance.");
        case BrowseMode::ShaderPacks:
            return QObject::tr("Search and install shader packs for a selected instance.");
    }
    return {};
}

static QString modeIconName(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:      return QStringLiteral("new");
        case BrowseMode::Mods:          return QStringLiteral("centralmods");
        case BrowseMode::ResourcePacks: return QStringLiteral("resourcepacks");
        case BrowseMode::ShaderPacks:   return QStringLiteral("shaderpacks");
    }
    return {};
}

void BrowsePage::buildLayout()
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Scroll area so the page works on small windows
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName(QStringLiteral("browseScroll"));

    auto* content = new QWidget(scroll);
    content->setObjectName(QStringLiteral("browsePageContent"));
    scroll->setWidget(content);

    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(40, 36, 40, 40);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop);

    // ── Header ───────────────────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout();
    headerRow->setSpacing(16);

    auto* iconLbl = new QLabel(content);
    iconLbl->setFixedSize(48, 48);
    iconLbl->setPixmap(QIcon::fromTheme(modeIconName(m_mode)).pixmap(48, 48));
    headerRow->addWidget(iconLbl);

    auto* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);
    m_titleLabel = new QLabel(modeTitle(m_mode), content);
    m_titleLabel->setObjectName(QStringLiteral("browsePageTitle"));
    m_descLabel = new QLabel(modeDesc(m_mode), content);
    m_descLabel->setObjectName(QStringLiteral("browsePageDesc"));
    m_descLabel->setWordWrap(true);
    titleCol->addWidget(m_titleLabel);
    titleCol->addWidget(m_descLabel);
    headerRow->addLayout(titleCol, 1);
    layout->addLayout(headerRow);

    // Separator
    auto* sep = new QFrame(content);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("browsePageSep"));
    layout->addWidget(sep);

    // ── Instance selector (non-Modpacks) ─────────────────────────────────────
    if (m_mode != BrowseMode::Modpacks) {
        auto* instRow = new QHBoxLayout();
        instRow->setSpacing(12);

        auto* instLbl = new QLabel(tr("Install into:"), content);
        instLbl->setObjectName(QStringLiteral("browsePageLabel"));
        instRow->addWidget(instLbl);

        m_instanceCombo = new QComboBox(content);
        m_instanceCombo->setObjectName(QStringLiteral("browseInstanceCombo"));
        m_instanceCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        instRow->addWidget(m_instanceCombo, 1);

        layout->addLayout(instRow);
        updateInstanceCombo();
    }

    // ── Search bar (not shown for Modpacks — it has its own search inside the dialog) ───
    if (m_mode != BrowseMode::Modpacks) {
        m_searchEdit = new QLineEdit(content);
        m_searchEdit->setObjectName(QStringLiteral("browseSearchEdit"));
        m_searchEdit->setPlaceholderText(tr("Search..."));
        m_searchEdit->setFixedHeight(42);
        connect(m_searchEdit, &QLineEdit::returnPressed, this,
                [this] { onPlatformClicked(QStringLiteral("modrinth")); });
        layout->addWidget(m_searchEdit);
    }

    // ── Platform source label ─────────────────────────────────────────────────
    m_pickLabel = new QLabel(tr("Choose a source:"), content);
    m_pickLabel->setObjectName(QStringLiteral("browsePickLabel"));
    layout->addWidget(m_pickLabel);

    // ── Platform cards ────────────────────────────────────────────────────────
    addPlatformCards(layout);

    layout->addStretch(1);
    outer->addWidget(scroll);
}

void BrowsePage::addPlatformCards(QLayout* parentLayout)
{
    const auto platforms = platformsForMode(m_mode);

    // Two cards per row using a flow of HBoxLayouts
    QHBoxLayout* rowLayout = nullptr;
    int col = 0;
    constexpr int COLS = 2;

    for (const Platform& plat : platforms) {
        if (col == 0 || rowLayout == nullptr) {
            rowLayout = new QHBoxLayout();
            rowLayout->setSpacing(12);
            parentLayout->addItem(rowLayout);
        }

        // ── Platform card ─────────────────────────────────────────────────────
        auto* card = new QFrame(parentLayout->parentWidget());
        card->setObjectName(QStringLiteral("platformCard"));
        card->setCursor(Qt::PointingHandCursor);
        card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        card->setFixedHeight(78);

        auto* cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);
        cardLayout->setSpacing(14);

        // Icon
        auto* iconLbl = new QLabel(card);
        iconLbl->setFixedSize(36, 36);
        const QIcon icon = QIcon::fromTheme(plat.iconName);
        if (!icon.isNull())
            iconLbl->setPixmap(icon.pixmap(36, 36));
        cardLayout->addWidget(iconLbl);

        // Name + description
        auto* textCol = new QVBoxLayout();
        textCol->setSpacing(3);
        auto* nameLbl = new QLabel(plat.name, card);
        nameLbl->setObjectName(QStringLiteral("platformCardName"));
        auto* descLbl = new QLabel(plat.desc, card);
        descLbl->setObjectName(QStringLiteral("platformCardDesc"));
        descLbl->setWordWrap(true);
        textCol->addWidget(nameLbl);
        textCol->addWidget(descLbl);
        cardLayout->addLayout(textCol, 1);

        // Chevron →
        auto* arrow = new QLabel(QStringLiteral("›"), card);
        arrow->setObjectName(QStringLiteral("platformCardArrow"));
        arrow->setFixedWidth(16);
        cardLayout->addWidget(arrow);

        // Click: mouse press on the card frame
        const QString pid = plat.id;
        card->installEventFilter(this);
        card->setProperty("platformId", pid);

        rowLayout->addWidget(card);
        ++col;

        if (col >= COLS) {
            col = 0;
            rowLayout = nullptr;
        }
    }

    // Pad the last row if it has only one card
    if (col == 1 && rowLayout)
        rowLayout->addStretch(1);
}

bool BrowsePage::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        const QString pid = qobject_cast<QWidget*>(obj) ? qobject_cast<QWidget*>(obj)->property("platformId").toString() : QString();
        if (!pid.isEmpty()) {
            onPlatformClicked(pid);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void BrowsePage::onPlatformClicked(const QString& platformId)
{
    const QString searchTerm = m_searchEdit ? m_searchEdit->text() : QString();
    QString instanceId;
    if (m_instanceCombo && m_instanceCombo->currentIndex() >= 0)
        instanceId = m_instanceCombo->currentData().toString();

    emit openBrowserRequested(m_mode, platformId, searchTerm, instanceId);
}

void BrowsePage::updateInstanceCombo()
{
    if (!m_instanceCombo)
        return;

    m_instanceCombo->clear();
    auto* instances = APPLICATION->instances();
    for (int i = 0; i < instances->count(); ++i) {
        auto inst = instances->at(i);
        if (inst)
            m_instanceCombo->addItem(inst->name(), inst->id());
    }

    if (m_instanceCombo->count() == 0)
        m_instanceCombo->addItem(tr("No instances available"), QString());
}

void BrowsePage::retranslate()
{
    if (m_titleLabel)  m_titleLabel->setText(modeTitle(m_mode));
    if (m_descLabel)   m_descLabel->setText(modeDesc(m_mode));
    if (m_searchEdit)  m_searchEdit->setPlaceholderText(tr("Search..."));  // null for Modpacks
    if (m_pickLabel)   m_pickLabel->setText(tr("Choose a source:"));
}
