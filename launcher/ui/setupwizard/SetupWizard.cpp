#include "SetupWizard.h"
#include "BaseWizardPage.h"
#include "WizardSidebar.h"

#include <Application.h>
#include <BuildConfig.h>
#include <QAbstractButton>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QShowEvent>

static const QString WIZARD_QSS = QStringLiteral(R"(
QWizard#SetupWizard {
    background: #1e1e2e;
}
/* Page content area */
QWizardPage {
    background: #1e1e2e;
    color: #cdd6f4;
}
/* Collapse the built-in title/subtitle — pages use their own header widgets */
QWizard QLabel#qt_wizard_title {
    font-size: 0px;
    max-height: 0px;
    padding: 0;
    margin: 0;
    color: #1e1e2e;
}
QWizard QLabel#qt_wizard_subtitle {
    font-size: 0px;
    max-height: 0px;
    padding: 0;
    margin: 0;
    color: #1e1e2e;
}
/* Button row */
QWizard QPushButton {
    background: #313244;
    color: #cdd6f4;
    border: 1px solid #45475a;
    border-radius: 6px;
    padding: 8px 22px;
    font-size: 13px;
    min-width: 80px;
}
QWizard QPushButton:hover  { background: #45475a; }
QWizard QPushButton:pressed { background: #585b70; }
QWizard QPushButton#qt_wizard_nextbutton,
QWizard QPushButton#qt_wizard_finishbutton {
    background: #cba6f7;
    color: #1e1e2e;
    border: none;
    font-weight: bold;
}
QWizard QPushButton#qt_wizard_nextbutton:hover,
QWizard QPushButton#qt_wizard_finishbutton:hover  { background: #d5b7f8; }
QWizard QPushButton#qt_wizard_nextbutton:pressed,
QWizard QPushButton#qt_wizard_finishbutton:pressed { background: #b891f5; }
/* Page header block */
QFrame#wizardPageHeader {
    background: #181825;
    border-bottom: 1px solid #313244;
}
QLabel#wizardPageTitle {
    color: #cdd6f4;
    font-size: 17px;
    font-weight: bold;
}
QLabel#wizardPageSubtitle {
    color: #a6adc8;
    font-size: 11px;
}
/* Option cards (AutoJava / Paste pages) */
QFrame#optionCard {
    background: #24273a;
    border: 2px solid #313244;
    border-radius: 8px;
}
QFrame#optionCard:hover {
    background: #2a2d3e;
    border-color: #45475a;
}
QRadioButton#cardRadio {
    color: #cdd6f4;
    font-size: 13px;
    font-weight: bold;
    spacing: 8px;
    background: transparent;
}
QRadioButton#cardRadio::indicator {
    width: 18px;
    height: 18px;
    border-radius: 9px;
    border: 2px solid #6c7086;
    background: transparent;
}
QRadioButton#cardRadio::indicator:checked {
    border-color: #cba6f7;
    background: #cba6f7;
}
QLabel#cardDesc {
    color: #a6adc8;
    font-size: 11px;
    background: transparent;
}
/* Login account buttons */
QPushButton#loginBtn {
    background: #24273a;
    color: #cdd6f4;
    border: 1px solid #313244;
    border-radius: 8px;
    padding: 14px 20px;
    text-align: left;
    font-size: 13px;
}
QPushButton#loginBtn:hover  { background: #313244; border-color: #45475a; }
QPushButton#loginBtn:pressed { background: #3a3d52; }
/* FlameAPI warning card */
QFrame#flameWarnCard {
    background: #2a1f2e;
    border: 1px solid #f38ba8;
    border-radius: 8px;
}
/* FlameAPI fetch button */
QPushButton#flameBtn {
    background: #f38ba8;
    color: #1e1e2e;
    border: none;
    border-radius: 8px;
    padding: 12px 24px;
    font-size: 13px;
    font-weight: bold;
    min-width: 200px;
}
QPushButton#flameBtn:hover  { background: #f5a0b5; }
QPushButton#flameBtn:pressed { background: #e07090; }
/* Scroll areas */
QScrollArea { background: transparent; border: none; }
QScrollBar:vertical {
    background: #181825;
    width: 6px;
    border-radius: 3px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #45475a;
    border-radius: 3px;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)");

// Paint the sidebar step list onto a QPixmap for use as ClassicStyle watermark
QPixmap SetupWizard::generateSidebarPixmap(int currentIndex)
{
    const int W = SIDEBAR_W;
    const int H = qMax(height(), 480);
    QPixmap pix(W, H);
    pix.fill(QColor(0x18, 0x18, 0x25));

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Right border
    p.setPen(QColor(0x31, 0x32, 0x44));
    p.drawLine(W - 1, 0, W - 1, H);

    // Launcher name
    QFont f = p.font();
    f.setPointSize(13);
    f.setBold(true);
    p.setFont(f);
    p.setPen(QColor(0xcb, 0xa6, 0xf7));
    p.drawText(QRect(18, 22, W - 36, 32), Qt::AlignLeft | Qt::AlignVCenter, BuildConfig.LAUNCHER_DISPLAYNAME);

    // "Quick Setup" subtitle
    f.setPointSize(9);
    f.setBold(false);
    p.setFont(f);
    p.setPen(QColor(0x6c, 0x70, 0x86));
    p.drawText(QRect(18, 54, W - 36, 22), Qt::AlignLeft | Qt::AlignVCenter, tr("Quick Setup"));

    // Divider
    p.setPen(QColor(0x31, 0x32, 0x44));
    p.drawLine(18, 88, W - 18, 88);

    // Steps
    const auto ids = pageIds();
    int y = 108;
    constexpr int STEP_H = 44;
    for (int i = 0; i < ids.size(); ++i) {
        auto* pg = page(ids[i]);
        const QString name = pg ? pg->title() : QString();
        const bool isCurrent = (i == currentIndex);
        const bool isDone = (i < currentIndex);

        // Row highlight
        if (isCurrent) {
            QPainterPath rowPath;
            rowPath.addRoundedRect(QRectF(10, y - 6, W - 20, STEP_H - 2), 6, 6);
            p.fillPath(rowPath, QColor(0x31, 0x32, 0x44, 200));
            p.fillRect(QRect(10, y - 6, 3, STEP_H - 2), QColor(0xcb, 0xa6, 0xf7));
        }

        // Numbered/done circle
        const QRectF circle(15, y + 3, 22, 22);
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
        p.drawText(QRect(46, y, W - 58, 28), Qt::AlignLeft | Qt::AlignVCenter, name);

        y += STEP_H;
    }

    p.end();
    return pix;
}

SetupWizard::SetupWizard(QWidget* parent) : QWizard(parent)
{
    setObjectName(QStringLiteral("SetupWizard"));
    // ClassicStyle renders WatermarkPixmap as the left column on all pages
    setWizardStyle(QWizard::ClassicStyle);
    setOptions(QWizard::NoCancelButton | QWizard::IndependentPages | QWizard::HaveCustomButton1);
    resize(840, 580);
    setMinimumSize(520, 460);

    setStyleSheet(WIZARD_QSS);

    retranslate();

    connect(this, &QWizard::currentIdChanged, this, &SetupWizard::pageChanged);
}

SetupWizard::~SetupWizard() = default;

void SetupWizard::resizeEvent(QResizeEvent* event)
{
    QWizard::resizeEvent(event);
    // Regenerate sidebar pixmap when the window is resized so it fills the full height
    const int currentIndex = pageIds().indexOf(currentId());
    setPixmap(QWizard::WatermarkPixmap, generateSidebarPixmap(currentIndex));
}

void SetupWizard::showEvent(QShowEvent* event)
{
    QWizard::showEvent(event);
    // Generate initial sidebar after all pages have been added
    const int currentIndex = pageIds().indexOf(currentId());
    setPixmap(QWizard::WatermarkPixmap, generateSidebarPixmap(currentIndex));
}

void SetupWizard::retranslate()
{
    setButtonText(QWizard::NextButton, tr("Next >"));
    setButtonText(QWizard::BackButton, tr("< Back"));
    setButtonText(QWizard::FinishButton, tr("Finish"));
    setButtonText(QWizard::CustomButton1, tr("Refresh"));
    setWindowTitle(tr("%1 Quick Setup").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
}

BaseWizardPage* SetupWizard::getBasePage(int id)
{
    if (id == -1)
        return nullptr;
    return dynamic_cast<BaseWizardPage*>(page(id));
}

BaseWizardPage* SetupWizard::getCurrentBasePage()
{
    return getBasePage(currentId());
}

void SetupWizard::pageChanged(int id)
{
    // Update sidebar to show new current step
    const int currentIndex = pageIds().indexOf(id);
    setPixmap(QWizard::WatermarkPixmap, generateSidebarPixmap(currentIndex));

    auto* basePagePtr = getBasePage(id);
    if (!basePagePtr)
        return;

    if (basePagePtr->wantsRefreshButton()) {
        setButtonLayout(
            { QWizard::CustomButton1, QWizard::Stretch, QWizard::BackButton, QWizard::NextButton, QWizard::FinishButton });
        auto* customButton = button(QWizard::CustomButton1);
        connect(customButton, &QAbstractButton::clicked, this, [this]() {
            if (auto* pg = getCurrentBasePage())
                pg->refresh();
        });
    } else {
        setButtonLayout({ QWizard::Stretch, QWizard::BackButton, QWizard::NextButton, QWizard::FinishButton });
    }
}

void SetupWizard::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWizard::changeEvent(event);
}
