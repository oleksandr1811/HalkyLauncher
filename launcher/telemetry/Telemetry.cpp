// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#include "Telemetry.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "Application.h"
#include "BuildConfig.h"
#include "settings/SettingsObject.h"

Telemetry::Telemetry(QObject* parent) : QObject(parent) {}

void Telemetry::sendLaunchPing()
{
    // Respect opt-out
    APPLICATION->settings()->getOrRegisterSetting(QStringLiteral("TelemetryEnabled"), true);
    if (!APPLICATION->settings()->get(QStringLiteral("TelemetryEnabled")).toBool())
        return;

    // Determine whether this is the very first launch
    APPLICATION->settings()->getOrRegisterSetting(QStringLiteral("TelemetryFirstRunSent"), false);
    const bool firstRun = !APPLICATION->settings()->get(QStringLiteral("TelemetryFirstRunSent")).toBool();

    QJsonObject payload;
    payload[QStringLiteral("type")]    = firstRun ? QStringLiteral("first-run") : QStringLiteral("run");
    payload[QStringLiteral("version")] = BuildConfig.versionString();

    const QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);

    QNetworkRequest req(QUrl(QStringLiteral(ENDPOINT)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    req.setHeader(QNetworkRequest::UserAgentHeader, BuildConfig.USER_AGENT);

    QNetworkReply* reply = APPLICATION->network()->post(req, body);
    // Fire-and-forget — just clean up the reply when done
    connect(reply, &QNetworkReply::finished, reply, [reply, firstRun]() {
        if (reply->error() == QNetworkReply::NoError && firstRun) {
            APPLICATION->settings()->set(QStringLiteral("TelemetryFirstRunSent"), true);
        }
        reply->deleteLater();
    });
}
