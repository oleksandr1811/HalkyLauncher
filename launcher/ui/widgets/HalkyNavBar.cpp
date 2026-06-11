// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "HalkyNavBar.h"

#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>

HalkyNavBar::HalkyNavBar(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("halkyNavBar"));
    setFixedWidth(COLLAPSED_W);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    buildLayout();
}

QToolButton* HalkyNavBar::makeNavButton(const QString& iconName, const QString& text)
{
    auto* btn = new QToolButton(this);
    btn->setObjectName(QStringLiteral("navItemBtn"));
    btn->setIcon(QIcon::fromTheme(iconName));
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setToolTip(text);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setFixedHeight(44);
    btn->setCheckable(true);
    btn->setAutoRaise(true);
    return btn;
}

QToolButton* HalkyNavBar::makeUtilButton(const QString& iconName, const QString& text)
{
    auto* btn = new QToolButton(this);
    btn->setObjectName(QStringLiteral("navActionBtn"));
    btn->setIcon(QIcon::fromTheme(iconName));
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setToolTip(text);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setFixedHeight(44);
    btn->setAutoRaise(true);
    return btn;
}

void HalkyNavBar::buildLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 10, 8, 10);
    m_mainLayout->setSpacing(4);

    // Collapse/expand toggle button
    m_toggleBtn = new QToolButton(this);
    m_toggleBtn->setObjectName(QStringLiteral("navToggleBtn"));
    m_toggleBtn->setIcon(QIcon::fromTheme(QStringLiteral("application-menu")));
    m_toggleBtn->setToolTip(tr("Expand navigation"));
    m_toggleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_toggleBtn->setFixedHeight(44);
    m_toggleBtn->setAutoRaise(true);
    connect(m_toggleBtn, &QToolButton::clicked, this, &HalkyNavBar::toggleExpand);
    m_mainLayout->addWidget(m_toggleBtn);

    // Separator
    auto* sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setObjectName(QStringLiteral("navSeparator"));
    m_mainLayout->addWidget(sep1);
    m_mainLayout->addSpacing(4);

    // Helper to register and add nav items
    auto addNavItem = [&](Page page, const QString& icon, const QString& text) {
        auto* btn = makeNavButton(icon, text);
        connect(btn, &QToolButton::clicked, this, [this, page]() {
            setCurrentPage(page);
            emit pageSelected(static_cast<int>(page));
        });
        m_navItems.append({ page, btn, text });
        m_mainLayout->addWidget(btn);
    };

    addNavItem(HomePage, QStringLiteral("home"), tr("Home"));
    addNavItem(LibraryPage, QStringLiteral("launcher"), tr("Library"));
    addNavItem(ModpacksPage, QStringLiteral("modrinth"), tr("Modpacks"));
    addNavItem(ModsPage, QStringLiteral("centralmods"), tr("Mods"));
    addNavItem(ResourcePacksPage, QStringLiteral("resourcepack"), tr("Resource Packs"));
    addNavItem(ShaderPacksPage, QStringLiteral("shaderpack"), tr("Shaders"));

    // Stretch to push utility buttons to bottom
    m_mainLayout->addStretch(1);

    // Separator before utility buttons
    auto* sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName(QStringLiteral("navSeparator"));
    m_mainLayout->addWidget(sep2);
    m_mainLayout->addSpacing(4);

    // Utility buttons
    m_addBtn = makeUtilButton(QStringLiteral("new"), tr("Add Instance"));
    connect(m_addBtn, &QToolButton::clicked, this, &HalkyNavBar::addInstanceClicked);
    m_mainLayout->addWidget(m_addBtn);

    m_accountsBtn = makeUtilButton(QStringLiteral("accounts"), tr("Accounts"));
    connect(m_accountsBtn, &QToolButton::clicked, this, &HalkyNavBar::accountsClicked);
    m_mainLayout->addWidget(m_accountsBtn);

    m_foldersBtn = makeUtilButton(QStringLiteral("viewfolder"), tr("Folders"));
    connect(m_foldersBtn, &QToolButton::clicked, this, &HalkyNavBar::foldersClicked);
    m_mainLayout->addWidget(m_foldersBtn);

    m_settingsBtn = makeUtilButton(QStringLiteral("settings"), tr("Settings"));
    connect(m_settingsBtn, &QToolButton::clicked, this, &HalkyNavBar::settingsClicked);
    m_mainLayout->addWidget(m_settingsBtn);

    m_helpBtn = makeUtilButton(QStringLiteral("help"), tr("Help"));
    connect(m_helpBtn, &QToolButton::clicked, this, &HalkyNavBar::helpClicked);
    m_mainLayout->addWidget(m_helpBtn);

    updateActiveState();
}

void HalkyNavBar::setCurrentPage(Page page)
{
    m_currentPage = page;
    updateActiveState();
}

void HalkyNavBar::toggleExpand()
{
    m_expanded = !m_expanded;
    const auto style = m_expanded ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;

    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));

    for (auto& item : m_navItems) {
        item.btn->setToolButtonStyle(style);
        item.btn->setToolTip(m_expanded ? QString() : item.defaultText);
    }

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

void HalkyNavBar::updateActiveState()
{
    for (auto& item : m_navItems)
        item.btn->setChecked(item.page == m_currentPage);
}

void HalkyNavBar::retranslate()
{
    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));

    const QStringList pageTexts = { tr("Home"),     tr("Library"),       tr("Modpacks"),
                                    tr("Mods"),     tr("Resource Packs"), tr("Shaders") };

    for (int i = 0; i < m_navItems.size() && i < pageTexts.size(); ++i) {
        m_navItems[i].defaultText = pageTexts[i];
        m_navItems[i].btn->setText(pageTexts[i]);
        if (!m_expanded)
            m_navItems[i].btn->setToolTip(pageTexts[i]);
    }

    const QStringList utilTexts = { tr("Add Instance"), tr("Accounts"), tr("Folders"),
                                    tr("Settings"),     tr("Help") };
    QList<QToolButton*> utilBtns = { m_addBtn, m_accountsBtn, m_foldersBtn, m_settingsBtn, m_helpBtn };
    for (int i = 0; i < utilBtns.size(); ++i) {
        utilBtns[i]->setText(utilTexts[i]);
        utilBtns[i]->setToolTip(m_expanded ? QString() : utilTexts[i]);
    }
}
