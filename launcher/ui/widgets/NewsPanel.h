// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#pragma once

#include <QEvent>
#include <QFrame>
#include <QList>
#include <QScrollArea>

#include "news/NewsEntry.h"

class QVBoxLayout;
class QLabel;
class QToolButton;

class NewsPanel : public QFrame {
    Q_OBJECT

   public:
    explicit NewsPanel(QWidget* parent = nullptr);

    void updateNews(const QList<NewsEntryPtr>& entries);
    void setLoading(bool loading);

    bool eventFilter(QObject* obj, QEvent* event) override;

   signals:
    void newsItemClicked(const QString& url);
    void moreNewsClicked();

   private:
    void buildLayout();
    void clearNews();
    void addNewsItem(const NewsEntryPtr& entry);

    QVBoxLayout* m_mainLayout = nullptr;
    QLabel* m_titleLabel = nullptr;
    QToolButton* m_moreBtn = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_scrollContent = nullptr;
    QVBoxLayout* m_newsLayout = nullptr;
    QLabel* m_emptyLabel = nullptr;

    static constexpr int PANEL_W = 260;
};
