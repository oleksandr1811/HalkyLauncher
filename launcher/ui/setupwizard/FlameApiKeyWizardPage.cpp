// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher - Minecraft Launcher
 *  Copyright (C) 2026 so5iso4ka <so5iso4ka@icloud.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "FlameApiKeyWizardPage.h"

#include <BuildConfig.h>
#include "Application.h"
#include "settings/SettingsObject.h"
#include "ui/GuiUtil.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QVBoxLayout>

FlameAPIKeyWizardPage::FlameAPIKeyWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Header
    auto* header = new QFrame(this);
    header->setObjectName(QStringLiteral("wizardPageHeader"));
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(28, 20, 28, 20);
    hLayout->setSpacing(20);
    auto* iconLabel = new QLabel(header);
    iconLabel->setFixedSize(52, 52);
    iconLabel->setPixmap(QIcon::fromTheme(QStringLiteral("new")).pixmap(52, 52));
    hLayout->addWidget(iconLabel);
    auto* textBox = new QVBoxLayout();
    textBox->setSpacing(6);
    m_headerTitle = new QLabel(header);
    m_headerTitle->setObjectName(QStringLiteral("wizardPageTitle"));
    m_headerSubtitle = new QLabel(header);
    m_headerSubtitle->setObjectName(QStringLiteral("wizardPageSubtitle"));
    m_headerSubtitle->setWordWrap(true);
    textBox->addWidget(m_headerTitle);
    textBox->addWidget(m_headerSubtitle);
    hLayout->addLayout(textBox, 1);
    outer->addWidget(header);

    // Content
    auto* content = new QVBoxLayout();
    content->setContentsMargins(24, 24, 24, 24);
    content->setSpacing(16);

    // Warning card
    auto* warnCard = new QFrame(this);
    warnCard->setStyleSheet(QStringLiteral(
        "QFrame { background: #2a1f2e; border: 1px solid #f38ba8; border-radius: 8px; padding: 12px; }"));
    auto* warnLayout = new QVBoxLayout(warnCard);
    warnLayout->setContentsMargins(16, 12, 16, 12);
    m_warnLabel = new QLabel(this);
    m_warnLabel->setWordWrap(true);
    m_warnLabel->setStyleSheet(QStringLiteral("color: #f38ba8; font-size: 12px; background: transparent; border: none;"));
    warnLayout->addWidget(m_warnLabel);
    content->addWidget(warnCard);

    m_descLabel = new QLabel(this);
    m_descLabel->setWordWrap(true);
    m_descLabel->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 12px;"));
    content->addWidget(m_descLabel);

    m_fetchButton = new QPushButton(this);
    m_fetchButton->setObjectName(QStringLiteral("flameBtn"));
    m_fetchButton->setCursor(Qt::PointingHandCursor);
    content->addWidget(m_fetchButton, 0, Qt::AlignLeft);

    content->addStretch(1);
    outer->addLayout(content, 1);

    connect(m_fetchButton, &QPushButton::clicked, this, [this]() {
        const auto& apiKey = GuiUtil::fetchFlameKey(this);
        if (!apiKey.isEmpty()) {
            APPLICATION->settings()->set("FlameKeyOverride", apiKey);
            APPLICATION->updateCapabilities();
        }
    });

    retranslate();
}

void FlameAPIKeyWizardPage::initializePage()
{
    APPLICATION->settings()->set("FlameKeyShouldBeFetchedOnStartup", false);
}

void FlameAPIKeyWizardPage::retranslate()
{
    setTitle(tr("CurseForge API"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("CurseForge API Key"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("Enable full CurseForge modpack downloads."));
    if (m_warnLabel)
        m_warnLabel->setText(
            tr("Warning: Using the official CurseForge app's API key may violate CurseForge's terms of service."));
    if (m_descLabel)
        m_descLabel->setText(
            tr("Fetching the key allows %1 to download all mods in a modpack automatically, "
               "without requiring manual downloads. This can also be done later in Settings.")
                .arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    if (m_fetchButton)
        m_fetchButton->setText(tr("Fetch Official Launcher's Key"));
}
