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

#include "ElySkinChange.h"

#include <QFile>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "Application.h"

ElySkinChange::ElySkinChange(QString token, QString skinPath)
    : Task(), m_token(token), m_skinPath(skinPath), m_network(APPLICATION->network())
{
}

ElySkinChange::Ptr ElySkinChange::make(QString token, QString skinPath)
{
    return makeShared<ElySkinChange>(token, skinPath);
}

void ElySkinChange::executeTask()
{
    setStatus(tr("Uploading skin to ely.by..."));

    QFile* file = new QFile(m_skinPath);
    if (!file->open(QIODevice::ReadOnly)) {
        emitFailed(tr("Cannot open skin file: %1").arg(m_skinPath));
        return;
    }

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"skin.png\""));
    filePart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(filePart);

    QNetworkRequest request(QUrl("https://ely.by/skins/upload"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());

    m_reply.reset(m_network->post(request, multiPart));
    multiPart->setParent(m_reply.get());

    connect(m_reply.get(), &QNetworkReply::finished, this, &ElySkinChange::onUploadFinished);
}

void ElySkinChange::onUploadFinished()
{
    if (!m_reply) {
        emitFailed(tr("No reply from ely.by"));
        return;
    }

    if (m_reply->error() != QNetworkReply::NoError) {
        auto response = m_reply->readAll();
        qWarning() << "ElySkinChange: Upload failed:" << response;
        emitFailed(tr("Skin upload failed: %1").arg(m_reply->errorString()));
        return;
    }

    auto responseData = m_reply->readAll();
    m_reply.reset();

    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emitFailed(tr("Failed to parse upload response: %1").arg(parseError.errorString()));
        return;
    }

    auto obj = doc.object();
    if (obj.value("error").toString() != "success_skin_load") {
        emitFailed(tr("Upload error: %1").arg(responseData));
        return;
    }

    // Extract skinId from URL like "/skins/s12345" or "/skins/s12345/edit"
    auto skinUrl = obj.value("url").toString();
    if (skinUrl.isEmpty()) {
        emitFailed(tr("No skin URL in upload response"));
        return;
    }

    int sIdx = skinUrl.indexOf("/s");
    if (sIdx == -1) {
        emitFailed(tr("Invalid skin URL: %1").arg(skinUrl));
        return;
    }

    m_skinId = skinUrl.mid(sIdx + 2);
    // Remove trailing path components like "/edit"
    if (m_skinId.contains('/')) {
        m_skinId = m_skinId.left(m_skinId.indexOf('/'));
    }

    if (m_skinId.isEmpty()) {
        emitFailed(tr("Could not extract skin ID from URL: %1").arg(skinUrl));
        return;
    }

    qInfo() << "ElySkinChange: Skin uploaded, ID:" << m_skinId;
    performWearRequest();
}

void ElySkinChange::performWearRequest()
{
    setStatus(tr("Activating skin on ely.by..."));

    QUrlQuery query;
    query.addQueryItem("skinId", m_skinId);

    QNetworkRequest request(QUrl("https://ely.by/skins/wear"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    m_reply.reset(m_network->post(request, query.toString(QUrl::FullyEncoded).toUtf8()));

    connect(m_reply.get(), &QNetworkReply::finished, this, &ElySkinChange::onWearFinished);
}

void ElySkinChange::onWearFinished()
{
    if (!m_reply) {
        emitFailed(tr("No reply from ely.by"));
        return;
    }

    if (m_reply->error() != QNetworkReply::NoError) {
        auto response = m_reply->readAll();
        qWarning() << "ElySkinChange: Wear failed:" << response;
        emitFailed(tr("Skin activation failed: %1").arg(m_reply->errorString()));
        return;
    }

    auto responseData = m_reply->readAll();
    m_reply.reset();

    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emitFailed(tr("Failed to parse wear response: %1").arg(parseError.errorString()));
        return;
    }

    auto obj = doc.object();
    if (obj.value("error").toString() == "success_skin_change") {
        qInfo() << "ElySkinChange: Skin successfully changed on ely.by!";
        emitSucceeded();
    } else {
        emitFailed(tr("Failed to activate skin: %1").arg(responseData));
    }
}
