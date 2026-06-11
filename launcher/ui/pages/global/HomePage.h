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
class QVBoxLayout;
class QScrollArea;
class BaseInstance;

class HomePage : public QWidget {
    Q_OBJECT

   public:
    explicit HomePage(QWidget* parent = nullptr);

    void refresh();

   signals:
    void launchInstance(const QString& instanceId);
    void editInstance(const QString& instanceId);
    void addInstanceRequested();
    void viewLibraryRequested();

   private:
    void buildLayout();
    void refreshAccount();
    void refreshInstances();
    void buildInstanceCard(BaseInstance* inst);

    QLabel* m_welcomeLabel = nullptr;
    QLabel* m_accountLabel = nullptr;
    QLabel* m_accountAvatarLabel = nullptr;
    QWidget* m_instancesContainer = nullptr;
    QVBoxLayout* m_instancesLayout = nullptr;
    QLabel* m_emptyInstancesLabel = nullptr;
    QScrollArea* m_scrollArea = nullptr;
};
