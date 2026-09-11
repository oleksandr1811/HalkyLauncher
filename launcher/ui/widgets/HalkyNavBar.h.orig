// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#pragma once

#include <QFrame>
#include <QLabel>
#include <QList>
#include <QToolButton>

class QVBoxLayout;

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
    QToolButton* makeNavButton(const QIcon& icon, const QString& text);
    QToolButton* makeUtilButton(const QIcon& icon, const QString& text);
    void updateActiveState();

    // Returns a painted icon for items that have no theme equivalent
    static QIcon paintedIcon(const QString& type);

    QVBoxLayout* m_mainLayout = nullptr;

    // Header: launcher name label + toggle button
    QWidget* m_headerWidget = nullptr;
    QLabel* m_launcherLabel = nullptr;
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
