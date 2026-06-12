// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "BrowsePage.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "Application.h"
#include "InstanceList.h"
#include "BaseInstance.h"

BrowsePage::BrowsePage(BrowseMode mode, QWidget* parent) : QWidget(parent), m_mode(mode)
{
    setObjectName(QStringLiteral("browsePage"));
    buildLayout();
}

static QString modeTitle(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:     return QObject::tr("Browse Modpacks");
        case BrowseMode::Mods:         return QObject::tr("Browse Mods");
        case BrowseMode::ResourcePacks: return QObject::tr("Browse Resource Packs");
        case BrowseMode::ShaderPacks:  return QObject::tr("Browse Shader Packs");
    }
    return {};
}

static QString modeDescription(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:
            return QObject::tr("Search and install ready-made modpacks from Modrinth, CurseForge, FTB, ATLauncher and more.");
        case BrowseMode::Mods:
            return QObject::tr("Search and install mods from Modrinth and CurseForge for a selected instance.");
        case BrowseMode::ResourcePacks:
            return QObject::tr("Search and install resource packs from Modrinth and CurseForge for a selected instance.");
        case BrowseMode::ShaderPacks:
            return QObject::tr("Search and install shader packs from Modrinth and CurseForge for a selected instance.");
    }
    return {};
}

static QString modeIcon(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:     return QStringLiteral("modrinth");
        case BrowseMode::Mods:         return QStringLiteral("centralmods");
        case BrowseMode::ResourcePacks: return QStringLiteral("resourcepack");
        case BrowseMode::ShaderPacks:  return QStringLiteral("shaderpack");
    }
    return {};
}

static QString modeBrowseButtonText(BrowseMode mode)
{
    switch (mode) {
        case BrowseMode::Modpacks:     return QObject::tr("Browse Modpacks...");
        case BrowseMode::Mods:         return QObject::tr("Browse Mods...");
        case BrowseMode::ResourcePacks: return QObject::tr("Browse Resource Packs...");
        case BrowseMode::ShaderPacks:  return QObject::tr("Browse Shader Packs...");
    }
    return {};
}

void BrowsePage::buildLayout()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto* content = new QWidget(this);
    content->setObjectName(QStringLiteral("browsePageContent"));
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop);

    // Header row with icon + title
    auto* headerRow = new QWidget(content);
    auto* headerLayout = new QHBoxLayout(headerRow);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(16);

    m_iconLabel = new QLabel(headerRow);
    m_iconLabel->setObjectName(QStringLiteral("browsePageIcon"));
    m_iconLabel->setFixedSize(48, 48);
    m_iconLabel->setPixmap(QIcon::fromTheme(modeIcon(m_mode)).pixmap(48, 48));
    headerLayout->addWidget(m_iconLabel);

    auto* titleCol = new QWidget(headerRow);
    auto* titleColLayout = new QVBoxLayout(titleCol);
    titleColLayout->setContentsMargins(0, 0, 0, 0);
    titleColLayout->setSpacing(4);

    m_titleLabel = new QLabel(modeTitle(m_mode), titleCol);
    m_titleLabel->setObjectName(QStringLiteral("browsePageTitle"));
    titleColLayout->addWidget(m_titleLabel);

    m_descLabel = new QLabel(modeDescription(m_mode), titleCol);
    m_descLabel->setObjectName(QStringLiteral("browsePageDesc"));
    m_descLabel->setWordWrap(true);
    titleColLayout->addWidget(m_descLabel);

    headerLayout->addWidget(titleCol, 1);
    layout->addWidget(headerRow);

    // Separator
    auto* sep = new QFrame(content);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("browsePageSep"));
    layout->addWidget(sep);

    // Instance selector (only for non-modpack modes)
    if (m_mode != BrowseMode::Modpacks) {
        auto* instanceRow = new QWidget(content);
        auto* instLayout = new QHBoxLayout(instanceRow);
        instLayout->setContentsMargins(0, 0, 0, 0);
        instLayout->setSpacing(12);

        auto* instLabel = new QLabel(tr("For instance:"), instanceRow);
        instLabel->setObjectName(QStringLiteral("browsePageLabel"));
        instLayout->addWidget(instLabel);

        m_instanceCombo = new QComboBox(instanceRow);
        m_instanceCombo->setObjectName(QStringLiteral("browseInstanceCombo"));
        m_instanceCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        instLayout->addWidget(m_instanceCombo, 1);

        layout->addWidget(instanceRow);
        updateInstanceCombo();
    }

    // Search bar row
    auto* searchRow = new QWidget(content);
    auto* searchLayout = new QHBoxLayout(searchRow);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(12);

    m_searchEdit = new QLineEdit(searchRow);
    m_searchEdit->setObjectName(QStringLiteral("browseSearchEdit"));
    m_searchEdit->setPlaceholderText(tr("Search..."));
    m_searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_searchEdit->setFixedHeight(40);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &BrowsePage::onBrowseClicked);
    searchLayout->addWidget(m_searchEdit, 1);

    m_browseBtn = new QPushButton(modeBrowseButtonText(m_mode), searchRow);
    m_browseBtn->setObjectName(QStringLiteral("browseBrowseBtn"));
    m_browseBtn->setFixedHeight(40);
    connect(m_browseBtn, &QPushButton::clicked, this, &BrowsePage::onBrowseClicked);
    searchLayout->addWidget(m_browseBtn);

    layout->addWidget(searchRow);

    // Tip label
    m_tipLabel = new QLabel(content);
    m_tipLabel->setObjectName(QStringLiteral("browsePageTip"));
    m_tipLabel->setWordWrap(true);

    if (m_mode == BrowseMode::Modpacks) {
        m_tipLabel->setText(tr("Tip: Clicking 'Browse Modpacks...' will open the full modpack browser "
                               "where you can install any pack from Modrinth, CurseForge, FTB and more."));
    } else {
        m_tipLabel->setText(tr("Tip: Select an instance first, then click Browse to open the resource "
                               "browser for that instance."));
    }
    layout->addWidget(m_tipLabel);

    layout->addStretch(1);

    outerLayout->addWidget(content);
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

void BrowsePage::onBrowseClicked()
{
    const QString searchTerm = m_searchEdit ? m_searchEdit->text() : QString();
    QString instanceId;
    if (m_instanceCombo && m_instanceCombo->currentIndex() >= 0)
        instanceId = m_instanceCombo->currentData().toString();

    emit openBrowserRequested(m_mode, searchTerm, instanceId);
}

void BrowsePage::retranslate()
{
    m_titleLabel->setText(modeTitle(m_mode));
    m_descLabel->setText(modeDescription(m_mode));
    m_browseBtn->setText(modeBrowseButtonText(m_mode));
    if (m_searchEdit)
        m_searchEdit->setPlaceholderText(tr("Search..."));
}
