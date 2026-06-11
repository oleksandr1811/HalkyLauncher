// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "HomePage.h"

#include <QDateTime>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"
#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"

static constexpr int MAX_RECENT = 5;

HomePage::HomePage(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("homePage"));
    buildLayout();
}

void HomePage::buildLayout()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // Scroll area wrapping everything
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName(QStringLiteral("homeScrollArea"));
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* content = new QWidget(m_scrollArea);
    content->setObjectName(QStringLiteral("homeContent"));
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(32, 32, 32, 32);
    contentLayout->setSpacing(0);

    // Welcome header
    auto* headerWidget = new QWidget(content);
    headerWidget->setObjectName(QStringLiteral("homeHeader"));
    auto* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(16);

    // Account avatar placeholder
    m_accountAvatarLabel = new QLabel(headerWidget);
    m_accountAvatarLabel->setObjectName(QStringLiteral("homeAvatar"));
    m_accountAvatarLabel->setFixedSize(48, 48);
    m_accountAvatarLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_accountAvatarLabel);

    auto* titleGroup = new QWidget(headerWidget);
    auto* titleGroupLayout = new QVBoxLayout(titleGroup);
    titleGroupLayout->setContentsMargins(0, 0, 0, 0);
    titleGroupLayout->setSpacing(2);

    m_welcomeLabel = new QLabel(titleGroup);
    m_welcomeLabel->setObjectName(QStringLiteral("homeWelcomeLabel"));
    titleGroupLayout->addWidget(m_welcomeLabel);

    m_accountLabel = new QLabel(titleGroup);
    m_accountLabel->setObjectName(QStringLiteral("homeAccountLabel"));
    titleGroupLayout->addWidget(m_accountLabel);

    headerLayout->addWidget(titleGroup, 1);
    contentLayout->addWidget(headerWidget);
    contentLayout->addSpacing(24);

    // Recent instances section
    auto* recentHeader = new QWidget(content);
    auto* recentHeaderLayout = new QHBoxLayout(recentHeader);
    recentHeaderLayout->setContentsMargins(0, 0, 0, 0);

    auto* recentLabel = new QLabel(tr("Recently Played"), recentHeader);
    recentLabel->setObjectName(QStringLiteral("homeSectionTitle"));
    recentHeaderLayout->addWidget(recentLabel);
    recentHeaderLayout->addStretch();

    auto* viewAllBtn = new QPushButton(tr("View All"), recentHeader);
    viewAllBtn->setObjectName(QStringLiteral("homeLinkBtn"));
    viewAllBtn->setFlat(true);
    connect(viewAllBtn, &QPushButton::clicked, this, &HomePage::viewLibraryRequested);
    recentHeaderLayout->addWidget(viewAllBtn);

    contentLayout->addWidget(recentHeader);
    contentLayout->addSpacing(12);

    // Instances container
    m_instancesContainer = new QWidget(content);
    m_instancesContainer->setObjectName(QStringLiteral("homeInstancesContainer"));
    m_instancesLayout = new QVBoxLayout(m_instancesContainer);
    m_instancesLayout->setContentsMargins(0, 0, 0, 0);
    m_instancesLayout->setSpacing(8);

    m_emptyInstancesLabel = new QLabel(
        tr("No instances yet. Click the + button in the sidebar or use the button below to add one!"),
        m_instancesContainer);
    m_emptyInstancesLabel->setObjectName(QStringLiteral("homeEmptyLabel"));
    m_emptyInstancesLabel->setWordWrap(true);
    m_emptyInstancesLabel->setAlignment(Qt::AlignCenter);
    m_instancesLayout->addWidget(m_emptyInstancesLabel);

    contentLayout->addWidget(m_instancesContainer);
    contentLayout->addSpacing(20);

    // Add Instance button
    auto* addBtn = new QPushButton(tr("+ Add Instance"), content);
    addBtn->setObjectName(QStringLiteral("homeAddBtn"));
    addBtn->setFixedHeight(44);
    connect(addBtn, &QPushButton::clicked, this, &HomePage::addInstanceRequested);
    contentLayout->addWidget(addBtn);

    contentLayout->addStretch(1);

    m_scrollArea->setWidget(content);
    outerLayout->addWidget(m_scrollArea);
}

void HomePage::refresh()
{
    refreshAccount();
    refreshInstances();
}

void HomePage::refreshAccount()
{
    auto* accounts = APPLICATION->accounts();
    auto account = accounts->defaultAccount();

    if (account) {
        const QString name = account->profileName().isEmpty() ? account->displayName() : account->profileName();
        m_welcomeLabel->setText(tr("Welcome back, %1!").arg(name));
        m_accountLabel->setText(tr("Playing as %1").arg(name));

        const QPixmap face = account->getFace(48, 48);
        if (!face.isNull()) {
            m_accountAvatarLabel->setPixmap(face);
        } else {
            m_accountAvatarLabel->setPixmap(QIcon::fromTheme(QStringLiteral("noaccount")).pixmap(48, 48));
        }
    } else {
        m_welcomeLabel->setText(tr("Welcome to Halky Launcher!"));
        m_accountLabel->setText(tr("No account selected"));
        m_accountAvatarLabel->setPixmap(QIcon::fromTheme(QStringLiteral("noaccount")).pixmap(48, 48));
    }
}

void HomePage::refreshInstances()
{
    // Clear existing instance cards
    while (m_instancesLayout->count() > 0) {
        auto* item = m_instancesLayout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    auto* instances = APPLICATION->instances();
    const int total = instances->count();

    if (total == 0) {
        auto* emptyLbl = new QLabel(
            tr("No instances yet. Use the + button in the sidebar or click 'Add Instance' to get started."),
            m_instancesContainer);
        emptyLbl->setObjectName(QStringLiteral("homeEmptyLabel"));
        emptyLbl->setWordWrap(true);
        emptyLbl->setAlignment(Qt::AlignCenter);
        m_instancesLayout->addWidget(emptyLbl);
        return;
    }

    // Collect instances sorted by last played (most recent first)
    QList<BaseInstance*> sorted;
    for (int i = 0; i < total; ++i) {
        auto* inst = instances->at(i);
        if (inst)
            sorted.append(inst);
    }
    std::sort(sorted.begin(), sorted.end(), [](BaseInstance* a, BaseInstance* b) {
        return a->lastLaunch() > b->lastLaunch();
    });

    const int shown = qMin(sorted.size(), MAX_RECENT);
    for (int i = 0; i < shown; ++i)
        buildInstanceCard(sorted[i]);
}

void HomePage::buildInstanceCard(BaseInstance* inst)
{
    auto* card = new QWidget(m_instancesContainer);
    card->setObjectName(QStringLiteral("homeInstanceCard"));
    auto* layout = new QHBoxLayout(card);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(16);

    // Instance icon
    auto* iconLabel = new QLabel(card);
    iconLabel->setObjectName(QStringLiteral("homeInstIcon"));
    iconLabel->setFixedSize(40, 40);
    const auto icon = QIcon::fromTheme(inst->iconKey());
    iconLabel->setPixmap(icon.pixmap(40, 40));
    layout->addWidget(iconLabel);

    // Instance info (name + last played)
    auto* infoWidget = new QWidget(card);
    auto* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(2);

    auto* nameLabel = new QLabel(inst->name(), infoWidget);
    nameLabel->setObjectName(QStringLiteral("homeInstName"));
    infoLayout->addWidget(nameLabel);

    // Last played time
    const auto lastLaunch = inst->lastLaunch();
    QString lastPlayedStr;
    if (lastLaunch > 0) {
        const auto dt = QDateTime::fromSecsSinceEpoch(lastLaunch);
        lastPlayedStr = tr("Last played: %1").arg(dt.toString(QStringLiteral("dd MMM yyyy hh:mm")));
    } else {
        lastPlayedStr = tr("Never played");
    }
    auto* timeLabel = new QLabel(lastPlayedStr, infoWidget);
    timeLabel->setObjectName(QStringLiteral("homeInstTime"));
    infoLayout->addWidget(timeLabel);

    layout->addWidget(infoWidget, 1);

    // Launch button
    auto* launchBtn = new QPushButton(tr("Play"), card);
    launchBtn->setObjectName(QStringLiteral("homeInstLaunchBtn"));
    launchBtn->setFixedSize(80, 36);
    const QString instId = inst->id();
    connect(launchBtn, &QPushButton::clicked, this, [this, instId]() { emit launchInstance(instId); });
    layout->addWidget(launchBtn);

    m_instancesLayout->addWidget(card);
}
