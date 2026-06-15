// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#pragma once

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "ui/widgets/BrowsePage.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

class BaseInstance;
class MinecraftInstance;

// Hosts a ResourceDownloadDialog as an embedded widget (no separate window).
// Provides a live-updated "Install into:" instance combo at the top.
class InlineResourcePage : public QWidget {
    Q_OBJECT

   public:
    explicit InlineResourcePage(BrowseMode mode, QWidget* parent = nullptr);
    ~InlineResourcePage() override = default;

    // Selects the instance matching id in the combo (called by MainWindow when
    // the user navigates here from the Library with an instance already selected).
    void setCurrentInstanceById(const QString& id);

   private slots:
    void onInstanceComboChanged(int index);

   protected:
    void showEvent(QShowEvent* event) override;

   private:
    void buildLayout();
    void updateInstanceCombo();
    void showBrowserForInstance(BaseInstance* inst);
    void clearBrowser();
    void onDialogFinished(int result);
    void runDownloadTasks(const QList<ResourceDownload::ResourceDownloadDialog::DownloadTaskPtr>& tasks);

    BrowseMode m_mode;
    bool m_browserStretchPresent = true;
    QComboBox* m_instanceCombo = nullptr;
    QLabel* m_noInstanceLabel = nullptr;
    QWidget* m_browserArea = nullptr;
    QVBoxLayout* m_browserLayout = nullptr;
    ResourceDownload::ResourceDownloadDialog* m_dlg = nullptr;
};
