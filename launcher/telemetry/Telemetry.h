// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 */

#pragma once

#include <QObject>

/*!
 * Lightweight anonymous telemetry.
 *
 * Sends a single JSON POST to the telemetry endpoint on every launch.
 * Respects the "TelemetryEnabled" setting — if false, nothing is sent.
 *
 * Payload: { "type": "run" | "first-run", "version": "x.y.z" }
 */
class Telemetry : public QObject {
    Q_OBJECT

   public:
    explicit Telemetry(QObject* parent = nullptr);

    /// Call once after the application is fully initialised.
    void sendLaunchPing();

   private:
    static constexpr const char* ENDPOINT = "https://halkylauncher.alex1811.ovh/telemetry.php";
};
