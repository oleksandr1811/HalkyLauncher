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
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
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

    // ── News header (top) ─────────────────────────────────────────────────────
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

    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("newsPanelSep"));
    m_mainLayout->addWidget(sep);

    // ── News scroll area ──────────────────────────────────────────────────────
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

    // ── Ad banner (above profile, hidden until image loads) ───────────────────
    m_adContainer = new QWidget(this);
    m_adContainer->setObjectName(QStringLiteral("newsPanelAdContainer"));
    m_adContainer->setVisible(false);
    auto* adContainerLayout = new QVBoxLayout(m_adContainer);
    adContainerLayout->setContentsMargins(0, 0, 0, 0);
    adContainerLayout->setSpacing(0);

    auto* adSep = new QFrame(m_adContainer);
    adSep->setFrameShape(QFrame::HLine);
    adSep->setObjectName(QStringLiteral("newsPanelSep"));
    adContainerLayout->addWidget(adSep);

    m_adBanner = new QLabel(m_adContainer);
    m_adBanner->setObjectName(QStringLiteral("newsPanelAdBanner"));
    m_adBanner->setFixedSize(PANEL_W, PANEL_W);
    m_adBanner->setAlignment(Qt::AlignCenter);
    m_adBanner->setScaledContents(true);
    adContainerLayout->addWidget(m_adBanner);

    m_mainLayout->addWidget(m_adContainer);

    // ── Account section (bottom) ──────────────────────────────────────────────
    auto* accSep = new QFrame(this);
    accSep->setFrameShape(QFrame::HLine);
    accSep->setObjectName(QStringLiteral("newsPanelSep"));
    m_mainLayout->addWidget(accSep);

    m_accountWidget = new QWidget(this);
    m_accountWidget->setObjectName(QStringLiteral("newsPanelAccount"));
    m_accountWidget->setCursor(Qt::PointingHandCursor);
    m_accountWidget->setFixedHeight(56);

    auto* accLayout = new QHBoxLayout(m_accountWidget);
    accLayout->setContentsMargins(14, 8, 14, 8);
    accLayout->setSpacing(10);

    m_accountAvatar = new QLabel(m_accountWidget);
    m_accountAvatar->setObjectName(QStringLiteral("newsPanelAccountAvatar"));
    m_accountAvatar->setFixedSize(32, 32);
    accLayout->addWidget(m_accountAvatar);

    m_accountName = new QLabel(tr("No account"), m_accountWidget);
    m_accountName->setObjectName(QStringLiteral("newsPanelAccountName"));
    m_accountName->setWordWrap(false);
    accLayout->addWidget(m_accountName, 1);

    auto* switchBtn = new QToolButton(m_accountWidget);
    switchBtn->setObjectName(QStringLiteral("newsPanelAccountSwitch"));
    switchBtn->setText(QStringLiteral("▾"));
    switchBtn->setToolTip(tr("Switch account"));
    switchBtn->setAutoRaise(true);
    connect(switchBtn, &QToolButton::clicked, this, &NewsPanel::accountButtonClicked);
    accLayout->addWidget(switchBtn);

    m_accountWidget->installEventFilter(this);
    m_accountWidget->setProperty("accountRow", true);

    m_mainLayout->addWidget(m_accountWidget);

    loadAdBanner();
}

void NewsPanel::loadAdBanner()
{
    auto* manager = new QNetworkAccessManager(this);
    auto* reply = manager->get(QNetworkRequest(QUrl(QStringLiteral("https://halkylauncher.alex1811.ovh/ad.png"))));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() != QNetworkReply::NoError || status == 404) {
            m_adContainer->setVisible(false);
            return;
        }
        QPixmap pix;
        if (!pix.loadFromData(reply->readAll()) || pix.isNull()) {
            m_adContainer->setVisible(false);
            return;
        }
        m_adBanner->setPixmap(pix.scaled(PANEL_W, PANEL_W, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        m_adContainer->setVisible(true);
    });
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
        // Account row click → emit signal so MainWindow shows the account menu
        if (qobject_cast<QWidget*>(obj) && obj->property("accountRow").toBool()) {
            emit accountButtonClicked();
            return true;
        }
        const QString link = obj->property("newsLink").toString();
        if (!link.isEmpty())
            emit newsItemClicked(link);
    }
    return QFrame::eventFilter(obj, event);
}

void NewsPanel::setCurrentAccount(const QString& displayName, const QPixmap& face)
{
    if (!m_accountName || !m_accountAvatar)
        return;

    m_accountName->setText(displayName.isEmpty() ? tr("No account") : displayName);

    if (face.isNull()) {
        m_accountAvatar->setPixmap(
            QIcon::fromTheme(QStringLiteral("noaccount")).pixmap(32, 32));
    } else {
        m_accountAvatar->setPixmap(face.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
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
