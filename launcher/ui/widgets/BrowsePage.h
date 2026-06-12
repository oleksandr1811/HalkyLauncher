// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#pragma once

#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

enum class BrowseMode { Modpacks, Mods, ResourcePacks, ShaderPacks };

class BrowsePage : public QWidget {
    Q_OBJECT
   public:
    explicit BrowsePage(BrowseMode mode, QWidget* parent = nullptr);
    void retranslate();

   signals:
    // platformId: "modrinth", "flame", "ftb", "atl", "technic"
    void openBrowserRequested(BrowseMode mode, const QString& platformId,
                              const QString& searchTerm, const QString& instanceId);

   protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

   private:
    void buildLayout();
    void addPlatformCards(class QLayout* grid);
    void updateInstanceCombo();
    void onPlatformClicked(const QString& platformId);

    BrowseMode m_mode;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_descLabel = nullptr;
    QLabel* m_instanceLabel = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QComboBox* m_instanceCombo = nullptr;
    QLabel* m_pickLabel = nullptr;
};
