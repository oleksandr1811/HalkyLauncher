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

#include <QList>
#include <QPainter>
#include <QRect>
#include <QWidget>

class QLabel;
class QPushButton;

/*!
 * Lightweight onboarding overlay that shows step-by-step tooltips
 * pointing at key UI elements. Shown only on first launch.
 */
class OnboardingOverlay : public QWidget {
    Q_OBJECT

   public:
    struct Step {
        QString title;
        QString body;
        QRect targetRect;  ///< rectangle of the element being highlighted (in overlay/window coords)
    };

    explicit OnboardingOverlay(QWidget* parent = nullptr);

    void setSteps(const QList<Step>& steps);

   signals:
    void finished();

   protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

   private slots:
    void advance();
    void skipAll();

   private:
    void buildTooltipWidget();
    void positionTooltip();
    void applyStep();

    QList<Step> m_steps;
    int m_currentStep = 0;

    QWidget* m_tooltip = nullptr;
    QLabel* m_stepLabel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_bodyLabel = nullptr;
    QPushButton* m_nextBtn = nullptr;
    QPushButton* m_skipBtn = nullptr;
};
