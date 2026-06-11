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

#include <QWidget>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QVBoxLayout;

// BrowseMode determines what kind of content this page browses
// and which dialog to open when the user wants to install/browse
enum class BrowseMode {
    Modpacks,     // Opens NewInstanceDialog
    Mods,         // Opens ResourceDownloadDialog (mods) for selected instance
    ResourcePacks, // Opens ResourceDownloadDialog (resourcepacks) for selected instance
    ShaderPacks,  // Opens ResourceDownloadDialog (shaderpacks) for selected instance
};

class BrowsePage : public QWidget {
    Q_OBJECT

   public:
    explicit BrowsePage(BrowseMode mode, QWidget* parent = nullptr);
    void retranslate();

   signals:
    void openBrowserRequested(BrowseMode mode, const QString& searchTerm, const QString& instanceId);

   private:
    void buildLayout();
    void onBrowseClicked();
    void updateInstanceCombo();

    BrowseMode m_mode;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_descLabel = nullptr;
    QLabel* m_iconLabel = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QComboBox* m_instanceCombo = nullptr;  // only for Mods/ResourcePacks/ShaderPacks
    QPushButton* m_browseBtn = nullptr;
    QLabel* m_tipLabel = nullptr;
};
