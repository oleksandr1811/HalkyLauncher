// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#include "HalkyNavBar.h"

#include <BuildConfig.h>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QSizePolicy>
#include <QVBoxLayout>

// ── Inline icon painter for items without theme equivalents ──────────────────

QIcon HalkyNavBar::paintedIcon(const QString& type)
{
    constexpr int S = 20;
    QPixmap pix(S, S);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#a6adc8"));

    if (type == QLatin1String("home")) {
        // House: roof triangle + body rect + door
        QPolygonF roof;
        roof << QPointF(10, 1) << QPointF(19, 9) << QPointF(1, 9);
        p.drawPolygon(roof);
        p.drawRect(QRectF(2, 9, 16, 10));
        p.setBrush(QColor(0x18, 0x18, 0x25));
        p.drawRect(QRectF(7, 13, 6, 6));

    } else if (type == QLatin1String("library")) {
        // Grid of 4 squares (2×2) representing a collection
        for (int row = 0; row < 2; ++row)
            for (int col = 0; col < 2; ++col)
                p.drawRoundedRect(QRectF(1 + col * 10, 1 + row * 10, 8, 8), 2, 2);

    } else if (type == QLatin1String("modpacks")) {
        // Package / cube outline with a plus
        QPainterPath box;
        box.addRoundedRect(QRectF(2, 4, 16, 14), 3, 3);
        p.fillPath(box, QColor("#a6adc8"));
        // lid
        p.drawRect(QRectF(1, 2, 18, 4));
        p.setBrush(QColor(0x18, 0x18, 0x25));
        // cross
        p.drawRect(QRectF(9, 7, 2, 8));
        p.drawRect(QRectF(5, 10, 10, 2));

    } else if (type == QLatin1String("collapse")) {
        // Left-pointing chevron « for collapse
        p.setPen(QPen(QColor("#a6adc8"), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPainterPath ch;
        ch.moveTo(13, 4);
        ch.lineTo(7, 10);
        ch.lineTo(13, 16);
        p.drawPath(ch);
    } else if (type == QLatin1String("expand")) {
        // Right-pointing chevron » for expand / hamburger ≡
        p.setPen(QPen(QColor("#a6adc8"), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        // Hamburger ≡ — 3 horizontal lines
        for (int y : {5, 10, 15})
            p.drawLine(3, y, 17, y);
    }

    p.end();
    return QIcon(pix);
}

// ── Button factories ─────────────────────────────────────────────────────────

QToolButton* HalkyNavBar::makeNavButton(const QIcon& icon, const QString& text)
{
    auto* btn = new QToolButton(this);
    btn->setObjectName(QStringLiteral("navItemBtn"));
    btn->setIcon(icon);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setToolTip(text);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setFixedHeight(44);
    btn->setCheckable(true);
    btn->setAutoRaise(true);
    return btn;
}

QToolButton* HalkyNavBar::makeUtilButton(const QIcon& icon, const QString& text)
{
    auto* btn = new QToolButton(this);
    btn->setObjectName(QStringLiteral("navActionBtn"));
    btn->setIcon(icon);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setToolTip(text);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setFixedHeight(44);
    btn->setAutoRaise(true);
    return btn;
}

// ── Constructor ───────────────────────────────────────────────────────────────

HalkyNavBar::HalkyNavBar(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("halkyNavBar"));
    setFixedWidth(COLLAPSED_W);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    buildLayout();
}

// ── Layout builder ────────────────────────────────────────────────────────────

void HalkyNavBar::buildLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(6, 8, 6, 8);
    m_mainLayout->setSpacing(2);

    // ── Header row: [☰ toggle] [Launcher name label] ──────────────────────────
    m_headerWidget = new QWidget(this);
    m_headerWidget->setObjectName(QStringLiteral("navHeaderWidget"));
    auto* headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(6);

    m_toggleBtn = new QToolButton(this);
    m_toggleBtn->setObjectName(QStringLiteral("navToggleBtn"));
    m_toggleBtn->setIcon(paintedIcon(QStringLiteral("expand")));
    m_toggleBtn->setToolTip(tr("Expand navigation"));
    m_toggleBtn->setFixedSize(44, 44);
    m_toggleBtn->setAutoRaise(true);
    connect(m_toggleBtn, &QToolButton::clicked, this, &HalkyNavBar::toggleExpand);
    headerLayout->addWidget(m_toggleBtn);

    m_launcherLabel = new QLabel(BuildConfig.LAUNCHER_DISPLAYNAME, this);
    m_launcherLabel->setObjectName(QStringLiteral("navLauncherLabel"));
    m_launcherLabel->setVisible(false);  // shown only when expanded
    headerLayout->addWidget(m_launcherLabel, 1);

    m_mainLayout->addWidget(m_headerWidget);

    // Separator
    auto* sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setObjectName(QStringLiteral("navSeparator"));
    m_mainLayout->addWidget(sep1);
    m_mainLayout->addSpacing(2);

    // ── Nav items ─────────────────────────────────────────────────────────────
    auto addNavItem = [&](Page page, const QIcon& icon, const QString& text) {
        auto* btn = makeNavButton(icon, text);
        connect(btn, &QToolButton::clicked, this, [this, page]() {
            setCurrentPage(page);
            emit pageSelected(static_cast<int>(page));
        });
        m_navItems.append({ page, btn, text });
        m_mainLayout->addWidget(btn);
    };

    addNavItem(HomePage, paintedIcon(QStringLiteral("home")), tr("Home"));
    addNavItem(LibraryPage, paintedIcon(QStringLiteral("library")), tr("Library"));
    addNavItem(ModpacksPage, paintedIcon(QStringLiteral("modpacks")), tr("Modpacks"));
    addNavItem(ModsPage, QIcon::fromTheme(QStringLiteral("centralmods")), tr("Mods"));
    addNavItem(ResourcePacksPage, QIcon::fromTheme(QStringLiteral("resourcepacks")), tr("Resource Packs"));
    addNavItem(ShaderPacksPage, QIcon::fromTheme(QStringLiteral("shaderpacks")), tr("Shaders"));

    m_mainLayout->addStretch(1);

    // Separator before utility buttons
    auto* sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName(QStringLiteral("navSeparator"));
    m_mainLayout->addWidget(sep2);
    m_mainLayout->addSpacing(2);

    // ── Utility buttons ───────────────────────────────────────────────────────
    m_addBtn = makeUtilButton(QIcon::fromTheme(QStringLiteral("new")), tr("Add Instance"));
    connect(m_addBtn, &QToolButton::clicked, this, &HalkyNavBar::addInstanceClicked);
    m_mainLayout->addWidget(m_addBtn);

    m_accountsBtn = makeUtilButton(QIcon::fromTheme(QStringLiteral("accounts")), tr("Accounts"));
    connect(m_accountsBtn, &QToolButton::clicked, this, &HalkyNavBar::accountsClicked);
    m_mainLayout->addWidget(m_accountsBtn);

    m_foldersBtn = makeUtilButton(QIcon::fromTheme(QStringLiteral("viewfolder")), tr("Folders"));
    connect(m_foldersBtn, &QToolButton::clicked, this, &HalkyNavBar::foldersClicked);
    m_mainLayout->addWidget(m_foldersBtn);

    m_settingsBtn = makeUtilButton(QIcon::fromTheme(QStringLiteral("settings")), tr("Settings"));
    connect(m_settingsBtn, &QToolButton::clicked, this, &HalkyNavBar::settingsClicked);
    m_mainLayout->addWidget(m_settingsBtn);

    m_helpBtn = makeUtilButton(QIcon::fromTheme(QStringLiteral("help")), tr("Help"));
    connect(m_helpBtn, &QToolButton::clicked, this, &HalkyNavBar::helpClicked);
    m_mainLayout->addWidget(m_helpBtn);

    updateActiveState();
}

// ── Expand / collapse ─────────────────────────────────────────────────────────

void HalkyNavBar::toggleExpand()
{
    m_expanded = !m_expanded;
    const auto style = m_expanded ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;

    // Header: show/hide launcher name label
    m_launcherLabel->setVisible(m_expanded);
    m_toggleBtn->setIcon(paintedIcon(m_expanded ? QStringLiteral("collapse") : QStringLiteral("expand")));
    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));

    // Nav items
    for (auto& item : m_navItems) {
        item.btn->setToolButtonStyle(style);
        item.btn->setToolTip(m_expanded ? QString() : item.defaultText);
    }

    // Utility buttons
    auto applyUtil = [&](QToolButton* btn, const QString& label) {
        btn->setToolButtonStyle(style);
        btn->setToolTip(m_expanded ? QString() : label);
    };
    applyUtil(m_addBtn, tr("Add Instance"));
    applyUtil(m_accountsBtn, tr("Accounts"));
    applyUtil(m_foldersBtn, tr("Folders"));
    applyUtil(m_settingsBtn, tr("Settings"));
    applyUtil(m_helpBtn, tr("Help"));

    setFixedWidth(m_expanded ? EXPANDED_W : COLLAPSED_W);
}

// ── Page tracking ─────────────────────────────────────────────────────────────

void HalkyNavBar::setCurrentPage(Page page)
{
    m_currentPage = page;
    updateActiveState();
}

void HalkyNavBar::updateActiveState()
{
    for (auto& item : m_navItems)
        item.btn->setChecked(item.page == m_currentPage);
}

// ── Retranslate ───────────────────────────────────────────────────────────────

void HalkyNavBar::retranslate()
{
    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));
    if (m_launcherLabel)
        m_launcherLabel->setText(BuildConfig.LAUNCHER_DISPLAYNAME);

    const QStringList pageTexts = {
        tr("Home"), tr("Library"), tr("Modpacks"), tr("Mods"), tr("Resource Packs"), tr("Shaders")
    };
    for (int i = 0; i < m_navItems.size() && i < pageTexts.size(); ++i) {
        m_navItems[i].defaultText = pageTexts[i];
        m_navItems[i].btn->setText(pageTexts[i]);
        if (!m_expanded)
            m_navItems[i].btn->setToolTip(pageTexts[i]);
    }

    const QStringList utilTexts = { tr("Add Instance"), tr("Accounts"), tr("Folders"), tr("Settings"), tr("Help") };
    QList<QToolButton*> utilBtns = { m_addBtn, m_accountsBtn, m_foldersBtn, m_settingsBtn, m_helpBtn };
    for (int i = 0; i < utilBtns.size(); ++i) {
        utilBtns[i]->setText(utilTexts[i]);
        utilBtns[i]->setToolTip(m_expanded ? QString() : utilTexts[i]);
    }
}
