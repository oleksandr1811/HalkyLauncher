#include "WizardSidebar.h"

#include <BuildConfig.h>
#include <QPainter>
#include <QPainterPath>

WizardSidebar::WizardSidebar(QWidget* parent) : QWidget(parent)
{
    setFixedWidth(W);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void WizardSidebar::setSteps(const QList<Step>& steps)
{
    m_steps = steps;
    update();
}

void WizardSidebar::setCurrentStep(int index)
{
    m_currentStep = index;
    update();
}

void WizardSidebar::retranslate()
{
    update();
}

void WizardSidebar::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(rect(), QColor(0x18, 0x18, 0x25));

    // Right border
    p.setPen(QColor(0x31, 0x32, 0x44));
    p.drawLine(W - 1, 0, W - 1, height());

    // Launcher name
    QFont f = p.font();
    f.setPointSize(13);
    f.setBold(true);
    p.setFont(f);
    p.setPen(QColor(0xcb, 0xa6, 0xf7));
    p.drawText(QRect(18, 20, W - 36, 32), Qt::AlignLeft | Qt::AlignVCenter, BuildConfig.LAUNCHER_DISPLAYNAME);

    // "Quick Setup" subtitle
    f.setPointSize(9);
    f.setBold(false);
    p.setFont(f);
    p.setPen(QColor(0x6c, 0x70, 0x86));
    p.drawText(QRect(18, 52, W - 36, 22), Qt::AlignLeft | Qt::AlignVCenter, tr("Quick Setup"));

    // Divider
    p.setPen(QColor(0x31, 0x32, 0x44));
    p.drawLine(18, HEADER_H - 4, W - 18, HEADER_H - 4);

    if (m_steps.isEmpty())
        return;

    // Step list
    int y = HEADER_H + 8;
    for (int i = 0; i < m_steps.size(); ++i) {
        const bool isCurrent = (i == m_currentStep);
        const bool isDone = (i < m_currentStep);

        // Row highlight
        if (isCurrent) {
            QPainterPath rowPath;
            rowPath.addRoundedRect(QRectF(10, y - 6, W - 20, STEP_H - 4), 6, 6);
            p.fillPath(rowPath, QColor(0x31, 0x32, 0x44, 200));
            // Accent bar
            p.fillRect(QRect(10, y - 6, 3, STEP_H - 4), QColor(0xcb, 0xa6, 0xf7));
        }

        // Numbered/done circle
        const QRectF circle(16, y + 3, 22, 22);
        if (isDone) {
            p.setBrush(QColor(0xa6, 0xe3, 0xa1));
            p.setPen(Qt::NoPen);
            p.drawEllipse(circle);
            f.setPointSize(9);
            f.setBold(true);
            p.setFont(f);
            p.setPen(QColor(0x1e, 0x1e, 0x2e));
            p.drawText(circle.toRect(), Qt::AlignCenter, QStringLiteral("✓"));
        } else {
            p.setBrush(isCurrent ? QColor(0xcb, 0xa6, 0xf7) : QColor(0x31, 0x32, 0x44));
            p.setPen(Qt::NoPen);
            p.drawEllipse(circle);
            f.setPointSize(8);
            f.setBold(true);
            p.setFont(f);
            p.setPen(isCurrent ? QColor(0x1e, 0x1e, 0x2e) : QColor(0x6c, 0x70, 0x86));
            p.drawText(circle.toRect(), Qt::AlignCenter, QString::number(i + 1));
        }

        // Step name
        f.setPointSize(10);
        f.setBold(isCurrent);
        p.setFont(f);
        if (isDone)
            p.setPen(QColor(0xa6, 0xe3, 0xa1));
        else if (isCurrent)
            p.setPen(QColor(0xcd, 0xd6, 0xf4));
        else
            p.setPen(QColor(0x6c, 0x70, 0x86));
        p.drawText(QRect(46, y, W - 58, 28), Qt::AlignLeft | Qt::AlignVCenter, m_steps[i].name);

        y += STEP_H;
    }
}
