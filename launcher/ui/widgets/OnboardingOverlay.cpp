// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Halky Launcher
 *  Copyright (C) 2024-2025 Halky Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 */

#include "OnboardingOverlay.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

OnboardingOverlay::OnboardingOverlay(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("onboardingOverlay"));
    setAttribute(Qt::WA_TranslucentBackground);
    // Don't block mouse events for the rest of the window
    setWindowFlags(Qt::Widget);
    buildTooltipWidget();
}

void OnboardingOverlay::buildTooltipWidget()
{
    m_tooltip = new QWidget(this);
    m_tooltip->setObjectName(QStringLiteral("onboardingTooltip"));
    m_tooltip->setFixedWidth(320);

    auto* layout = new QVBoxLayout(m_tooltip);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    // Step indicator e.g. "1 / 4"
    m_stepLabel = new QLabel(m_tooltip);
    m_stepLabel->setObjectName(QStringLiteral("onboardingStep"));
    layout->addWidget(m_stepLabel);

    m_titleLabel = new QLabel(m_tooltip);
    m_titleLabel->setObjectName(QStringLiteral("onboardingTitle"));
    m_titleLabel->setWordWrap(true);
    layout->addWidget(m_titleLabel);

    m_bodyLabel = new QLabel(m_tooltip);
    m_bodyLabel->setObjectName(QStringLiteral("onboardingBody"));
    m_bodyLabel->setWordWrap(true);
    layout->addWidget(m_bodyLabel);

    // Button row
    auto* btnRow = new QWidget(m_tooltip);
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 4, 0, 0);
    btnLayout->setSpacing(8);

    m_skipBtn = new QPushButton(tr("Skip"), btnRow);
    m_skipBtn->setObjectName(QStringLiteral("onboardingSkipBtn"));
    m_skipBtn->setFlat(true);
    connect(m_skipBtn, &QPushButton::clicked, this, &OnboardingOverlay::skipAll);
    btnLayout->addWidget(m_skipBtn);

    btnLayout->addStretch();

    m_nextBtn = new QPushButton(tr("Next"), btnRow);
    m_nextBtn->setObjectName(QStringLiteral("onboardingNextBtn"));
    m_nextBtn->setDefault(true);
    connect(m_nextBtn, &QPushButton::clicked, this, &OnboardingOverlay::advance);
    btnLayout->addWidget(m_nextBtn);

    layout->addWidget(btnRow);
}

void OnboardingOverlay::setSteps(const QList<Step>& steps)
{
    m_steps = steps;
    m_currentStep = 0;
    applyStep();
}

void OnboardingOverlay::showEvent(QShowEvent*)
{
    if (parentWidget())
        resize(parentWidget()->size());
    positionTooltip();
    m_tooltip->show();
    m_tooltip->raise();
}

void OnboardingOverlay::resizeEvent(QResizeEvent*)
{
    positionTooltip();
}

void OnboardingOverlay::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect& target = (!m_steps.isEmpty() && m_currentStep < m_steps.size())
                              ? m_steps[m_currentStep].targetRect
                              : QRect();

    if (target.isValid()) {
        // Draw overlay as full rect minus the spotlight hole using a path with
        // an even-odd fill rule — this avoids CompositionMode_Clear which
        // renders as black instead of transparent on some platforms.
        QRect spotlight = target.adjusted(-6, -6, 6, 6);
        QPainterPath outerPath;
        outerPath.addRect(rect());
        QPainterPath innerPath;
        innerPath.addRoundedRect(spotlight, 8, 8);
        QPainterPath maskPath = outerPath.subtracted(innerPath);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 150));
        p.drawPath(maskPath);

        // Highlight border around spotlight
        p.setPen(QPen(QColor(203, 166, 247), 2));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(spotlight, 8, 8);
    } else {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
    }
}

void OnboardingOverlay::positionTooltip()
{
    if (!m_tooltip || m_steps.isEmpty())
        return;

    m_tooltip->adjustSize();
    const QSize ts = m_tooltip->size();
    const QSize ws = size();
    const int margin = 12;

    if (m_currentStep >= m_steps.size()) {
        m_tooltip->move((ws.width() - ts.width()) / 2, (ws.height() - ts.height()) / 2);
        return;
    }

    const QRect& target = m_steps[m_currentStep].targetRect;

    if (!target.isValid()) {
        m_tooltip->move((ws.width() - ts.width()) / 2, (ws.height() - ts.height()) / 2);
        return;
    }

    // Try positions in priority order: right, left, below, above
    // and pick the first one that fits fully inside the window.
    struct Candidate { int x; int y; };
    const QList<Candidate> candidates = {
        // Right of target, vertically aligned to target top
        { target.right() + margin, target.top() },
        // Left of target
        { target.left() - ts.width() - margin, target.top() },
        // Below target, horizontally centred
        { target.center().x() - ts.width() / 2, target.bottom() + margin },
        // Above target
        { target.center().x() - ts.width() / 2, target.top() - ts.height() - margin },
    };

    for (const auto& c : candidates) {
        int x = qBound(margin, c.x, ws.width()  - ts.width()  - margin);
        int y = qBound(margin, c.y, ws.height() - ts.height() - margin);
        // Accept this candidate if it doesn't overlap the spotlight
        QRect placed(x, y, ts.width(), ts.height());
        QRect spotlight = target.adjusted(-6, -6, 6, 6);
        if (!placed.intersects(spotlight)) {
            m_tooltip->move(x, y);
            return;
        }
    }

    // Fallback: bottom-right corner
    m_tooltip->move(
        qMax(margin, ws.width()  - ts.width()  - margin),
        qMax(margin, ws.height() - ts.height() - margin)
    );
}

void OnboardingOverlay::applyStep()
{
    if (m_steps.isEmpty()) {
        hide();
        emit finished();
        return;
    }

    const int total = m_steps.size();
    if (m_currentStep >= total) {
        hide();
        emit finished();
        return;
    }

    const Step& step = m_steps[m_currentStep];
    m_stepLabel->setText(tr("%1 / %2").arg(m_currentStep + 1).arg(total));
    m_titleLabel->setText(step.title);
    m_bodyLabel->setText(step.body);
    m_nextBtn->setText(m_currentStep + 1 < total ? tr("Next") : tr("Done"));

    update();      // repaint overlay with new spotlight
    positionTooltip();
}

void OnboardingOverlay::advance()
{
    ++m_currentStep;
    applyStep();
}

void OnboardingOverlay::skipAll()
{
    hide();
    emit finished();
}
