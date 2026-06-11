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

#include <QFrame>
#include <QList>
#include <QToolButton>

class QVBoxLayout;
class QPropertyAnimation;

class HalkyNavBar : public QFrame {
    Q_OBJECT

   public:
    enum Page {
        HomePage = 0,
        LibraryPage = 1,
        ModpacksPage = 2,
        ModsPage = 3,
        ResourcePacksPage = 4,
        ShaderPacksPage = 5,
        PageCount
    };

    explicit HalkyNavBar(QWidget* parent = nullptr);

    void setCurrentPage(Page page);
    Page currentPage() const { return m_currentPage; }
    bool isExpanded() const { return m_expanded; }
    void retranslate();

   signals:
    void pageSelected(int page);
    void addInstanceClicked();
    void settingsClicked();
    void foldersClicked();
    void accountsClicked();
    void helpClicked();

   public slots:
    void toggleExpand();

   private:
    void buildLayout();
    QToolButton* makeNavButton(const QString& iconName, const QString& text);
    QToolButton* makeUtilButton(const QString& iconName, const QString& text);
    void updateActiveState();

    QVBoxLayout* m_mainLayout = nullptr;
    QToolButton* m_toggleBtn = nullptr;

    struct NavItem {
        Page page;
        QToolButton* btn;
        QString defaultText;
    };
    QList<NavItem> m_navItems;

    QToolButton* m_addBtn = nullptr;
    QToolButton* m_accountsBtn = nullptr;
    QToolButton* m_foldersBtn = nullptr;
    QToolButton* m_settingsBtn = nullptr;
    QToolButton* m_helpBtn = nullptr;

    Page m_currentPage = HomePage;
    bool m_expanded = false;

    static constexpr int COLLAPSED_W = 62;
    static constexpr int EXPANDED_W = 220;
};
