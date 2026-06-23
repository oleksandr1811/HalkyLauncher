#include "LanguageWizardPage.h"

#include <Application.h>
#include <BuildConfig.h>
#include <QDesktopServices>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <translations/TranslationsModel.h>

#include "settings/SettingsObject.h"
#include "ui/widgets/LanguageSelectionWidget.h"

static QFrame* makePageHeader(QWidget* parent, QLabel*& titleOut, QLabel*& subtitleOut)
{
    auto* header = new QFrame(parent);
    header->setObjectName(QStringLiteral("wizardPageHeader"));
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(28, 20, 28, 20);
    hLayout->setSpacing(20);

    auto* iconLabel = new QLabel(header);
    iconLabel->setFixedSize(52, 52);
    iconLabel->setPixmap(QIcon::fromTheme(QStringLiteral("language")).pixmap(52, 52));
    hLayout->addWidget(iconLabel);

    auto* textBox = new QVBoxLayout();
    textBox->setSpacing(6);
    titleOut = new QLabel(header);
    titleOut->setObjectName(QStringLiteral("wizardPageTitle"));
    subtitleOut = new QLabel(header);
    subtitleOut->setObjectName(QStringLiteral("wizardPageSubtitle"));
    subtitleOut->setWordWrap(true);
    textBox->addWidget(titleOut);
    textBox->addWidget(subtitleOut);
    hLayout->addLayout(textBox, 1);

    return header;
}

LanguageWizardPage::LanguageWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    setObjectName(QStringLiteral("languagePage"));

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    outer->addWidget(makePageHeader(this, m_headerTitle, m_headerSubtitle));

    auto* content = new QVBoxLayout();
    content->setContentsMargins(24, 16, 24, 16);
    m_langWidget = new LanguageSelectionWidget(this);
    content->addWidget(m_langWidget);
    outer->addLayout(content, 1);

    // ── Telemetry notice footer ───────────────────────────────────────────
    auto* footer = new QWidget(this);
    footer->setObjectName(QStringLiteral("telemetryFooter"));
    auto* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(20, 8, 16, 10);
    footerLayout->setSpacing(6);

    m_telemetryStatusLabel = new QLabel(footer);
    m_telemetryStatusLabel->setObjectName(QStringLiteral("telemetryNotice"));
    m_telemetryStatusLabel->setWordWrap(true);
    footerLayout->addWidget(m_telemetryStatusLabel, 1);

    m_telemetryToggleBtn = new QToolButton(footer);
    m_telemetryToggleBtn->setObjectName(QStringLiteral("telemetryToggleBtn"));
    m_telemetryToggleBtn->setCursor(Qt::PointingHandCursor);
    m_telemetryToggleBtn->setAutoRaise(false);
    m_telemetryToggleBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_telemetryToggleBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    footerLayout->addWidget(m_telemetryToggleBtn);

    auto* infoBtn = new QToolButton(footer);
    infoBtn->setObjectName(QStringLiteral("telemetryInfoBtn"));
    infoBtn->setIcon(QIcon::fromTheme(QStringLiteral("help")));
    infoBtn->setIconSize(QSize(18, 18));
    infoBtn->setToolTip(QStringLiteral("Learn more about telemetry"));
    infoBtn->setCursor(Qt::PointingHandCursor);
    infoBtn->setAutoRaise(true);
    infoBtn->setFixedSize(28, 28);
    footerLayout->addWidget(infoBtn);

    outer->addWidget(footer);

    connect(m_telemetryToggleBtn, &QToolButton::clicked, this, [this]() {
        APPLICATION->settings()->getOrRegisterSetting(QStringLiteral("TelemetryEnabled"), true);
        APPLICATION->settings()->set(QStringLiteral("TelemetryEnabled"), !telemetryEnabled());
        updateTelemetryNotice();
    });
    // Click info button → open telemetry info page
    connect(infoBtn, &QToolButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://halkylauncher.alex1811.ovh/telemetry.html")));
    });
    m_infoBtn = infoBtn;

    retranslate();
}

LanguageWizardPage::~LanguageWizardPage() = default;

bool LanguageWizardPage::wantsRefreshButton()
{
    return true;
}

void LanguageWizardPage::refresh()
{
    APPLICATION->translations()->downloadIndex();
}

bool LanguageWizardPage::validatePage()
{
    APPLICATION->settings()->set("Language", m_langWidget->getSelectedLanguageKey());
    return true;
}

bool LanguageWizardPage::telemetryEnabled() const
{
    APPLICATION->settings()->getOrRegisterSetting(QStringLiteral("TelemetryEnabled"), true);
    return APPLICATION->settings()->get(QStringLiteral("TelemetryEnabled")).toBool();
}

void LanguageWizardPage::updateTelemetryNotice()
{
    if (!m_telemetryStatusLabel || !m_telemetryToggleBtn)
        return;

    if (telemetryEnabled()) {
        m_telemetryStatusLabel->setText(
            QStringLiteral("%1 sends anonymous telemetry.").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
        m_telemetryToggleBtn->setText(tr("Disable telemetry"));
    } else {
        m_telemetryStatusLabel->setText(tr("Telemetry is disabled."));
        m_telemetryToggleBtn->setText(tr("Enable telemetry"));
    }
}

void LanguageWizardPage::retranslate()
{
    setTitle(tr("Language"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Language"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("Select the language to use in %1").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    updateTelemetryNotice();
    if (m_infoBtn)
        m_infoBtn->setToolTip(QStringLiteral("Learn more about anonymous telemetry"));
    if (m_langWidget)
        m_langWidget->retranslate();
}
