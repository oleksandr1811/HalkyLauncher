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

    m_telemetryLabel = new QLabel(footer);
    m_telemetryLabel->setObjectName(QStringLiteral("telemetryNotice"));
    m_telemetryLabel->setCursor(Qt::PointingHandCursor);
    m_telemetryLabel->setTextFormat(Qt::RichText);
    m_telemetryLabel->setOpenExternalLinks(false);
    footerLayout->addWidget(m_telemetryLabel, 1);

    auto* infoBtn = new QToolButton(footer);
    infoBtn->setObjectName(QStringLiteral("telemetryInfoBtn"));
    infoBtn->setIcon(QIcon::fromTheme(QStringLiteral("help")));
    infoBtn->setIconSize(QSize(18, 18));
    infoBtn->setToolTip(tr("Learn more about telemetry"));
    infoBtn->setCursor(Qt::PointingHandCursor);
    infoBtn->setAutoRaise(true);
    infoBtn->setFixedSize(28, 28);
    footerLayout->addWidget(infoBtn);

    outer->addWidget(footer);

    // Click label → disable telemetry
    connect(m_telemetryLabel, &QLabel::linkActivated, this, [](const QString&) {
        APPLICATION->settings()->getOrRegisterSetting(QStringLiteral("TelemetryEnabled"), true);
        APPLICATION->settings()->set(QStringLiteral("TelemetryEnabled"), false);
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

void LanguageWizardPage::retranslate()
{
    setTitle(tr("Language"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Language"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("Select the language to use in %1").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    if (m_telemetryLabel)
        m_telemetryLabel->setText(
            tr("%1 sends anonymous telemetry. <a href=\"disable\">Click here to disable it.</a>")
                .arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    if (m_infoBtn)
        m_infoBtn->setToolTip(tr("Learn more about anonymous telemetry"));
    if (m_langWidget)
        m_langWidget->retranslate();
}
