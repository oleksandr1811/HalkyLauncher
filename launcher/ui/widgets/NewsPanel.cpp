// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "NewsPanel.h"

#include <QDesktopServices>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSizePolicy>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

NewsPanel::NewsPanel(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("newsPanel"));
    setFrameShape(QFrame::NoFrame);
    setFixedWidth(PANEL_W);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    buildLayout();
}

void NewsPanel::buildLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Panel header
    auto* header = new QWidget(this);
    header->setObjectName(QStringLiteral("newsPanelHeader"));
    header->setFixedHeight(48);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(14, 0, 8, 0);

    m_titleLabel = new QLabel(tr("News"), header);
    m_titleLabel->setObjectName(QStringLiteral("newsPanelTitle"));
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);

    m_moreBtn = new QToolButton(header);
    m_moreBtn->setObjectName(QStringLiteral("newsPanelMoreBtn"));
    m_moreBtn->setText(tr("More..."));
    m_moreBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_moreBtn->setAutoRaise(true);
    connect(m_moreBtn, &QToolButton::clicked, this, &NewsPanel::moreNewsClicked);
    headerLayout->addWidget(m_moreBtn);

    m_mainLayout->addWidget(header);

    // Separator
    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("newsPanelSep"));
    m_mainLayout->addWidget(sep);

    // Scroll area for news items
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName(QStringLiteral("newsPanelScroll"));
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scrollContent = new QWidget(m_scrollArea);
    m_scrollContent->setObjectName(QStringLiteral("newsPanelContent"));
    m_newsLayout = new QVBoxLayout(m_scrollContent);
    m_newsLayout->setContentsMargins(0, 0, 0, 0);
    m_newsLayout->setSpacing(1);
    m_newsLayout->setAlignment(Qt::AlignTop);

    m_emptyLabel = new QLabel(tr("No news available"), m_scrollContent);
    m_emptyLabel->setObjectName(QStringLiteral("newsPanelEmpty"));
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setWordWrap(true);
    m_newsLayout->addWidget(m_emptyLabel);
    m_newsLayout->addStretch(1);

    m_scrollArea->setWidget(m_scrollContent);
    m_mainLayout->addWidget(m_scrollArea, 1);
}

void NewsPanel::clearNews()
{
    while (m_newsLayout->count() > 0) {
        auto* item = m_newsLayout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
}

void NewsPanel::addNewsItem(const NewsEntryPtr& entry)
{
    auto* card = new QWidget(m_scrollContent);
    card->setObjectName(QStringLiteral("newsCard"));
    card->setCursor(Qt::PointingHandCursor);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);

    auto* titleLabel = new QLabel(card);
    titleLabel->setObjectName(QStringLiteral("newsCardTitle"));
    titleLabel->setText(entry->title);
    titleLabel->setWordWrap(true);
    layout->addWidget(titleLabel);

    // Short content preview (strip HTML, truncate)
    QString preview = entry->content;
    preview.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
    preview = preview.trimmed();
    if (preview.length() > 90)
        preview = preview.left(87) + QStringLiteral("...");

    if (!preview.isEmpty()) {
        auto* contentLabel = new QLabel(card);
        contentLabel->setObjectName(QStringLiteral("newsCardContent"));
        contentLabel->setText(preview);
        contentLabel->setWordWrap(true);
        layout->addWidget(contentLabel);
    }

    // Separator between cards
    auto* sep = new QFrame(m_scrollContent);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("newsCardSep"));

    m_newsLayout->addWidget(card);
    m_newsLayout->addWidget(sep);

    const QString link = entry->link;
    connect(card, &QWidget::destroyed, this, [link]() {});

    // Use event filter to make the whole card clickable
    card->installEventFilter(this);
    card->setProperty("newsLink", link);
}

bool NewsPanel::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        const QString link = obj->property("newsLink").toString();
        if (!link.isEmpty())
            emit newsItemClicked(link);
    }
    return QFrame::eventFilter(obj, event);
}

void NewsPanel::updateNews(const QList<NewsEntryPtr>& entries)
{
    clearNews();

    if (entries.isEmpty()) {
        auto* emptyLbl = new QLabel(tr("No news available"), m_scrollContent);
        emptyLbl->setObjectName(QStringLiteral("newsPanelEmpty"));
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setWordWrap(true);
        m_newsLayout->addWidget(emptyLbl);
        m_newsLayout->addStretch(1);
        return;
    }

    for (const auto& entry : entries)
        addNewsItem(entry);
    m_newsLayout->addStretch(1);
}

void NewsPanel::setLoading(bool loading)
{
    if (loading) {
        clearNews();
        auto* loadLabel = new QLabel(tr("Loading news..."), m_scrollContent);
        loadLabel->setObjectName(QStringLiteral("newsPanelEmpty"));
        loadLabel->setAlignment(Qt::AlignCenter);
        m_newsLayout->addWidget(loadLabel);
        m_newsLayout->addStretch(1);
    }
}
