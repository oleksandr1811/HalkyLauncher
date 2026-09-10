// SPDX-License-Identifier: GPL-3.0-only
/*
 *  HalkyLauncher - Minecraft Launcher
 *  Copyright (c) 2024 HalkyLauncher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>

#include "tasks/Task.h"

/**
 * Handles skin upload and activation on ely.by in two steps:
 * 1. Upload PNG to ely.by/skins/upload -> get skinId
 * 2. Activate skin via ely.by/skins/wear with the skinId
 */
class ElySkinChange : public Task {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<ElySkinChange>;

    ElySkinChange(QString token, QString skinPath);
    virtual ~ElySkinChange() = default;

    static ElySkinChange::Ptr make(QString token, QString skinPath);

   protected:
    void executeTask() override;

   private slots:
    void onUploadFinished();
    void onWearFinished();

   private:
    void performWearRequest();

    QString m_token;
    QString m_skinPath;
    QString m_skinId;
    QNetworkAccessManager* m_network;
    std::unique_ptr<QNetworkReply> m_reply;
};
