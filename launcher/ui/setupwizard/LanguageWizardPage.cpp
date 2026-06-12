#include "LanguageWizardPage.h"

#include <Application.h>
#include <BuildConfig.h>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <translations/TranslationsModel.h>

#include "settings/SettingsObject.h"
#include "ui/widgets/LanguageSelectionWidget.h"

static QFrame* makePageHeader(QWidget* parent, const QString& iconKey, QLabel*& titleOut, QLabel*& subtitleOut)
{
    auto* header = new QFrame(parent);
    header->setObjectName(QStringLiteral("wizardPageHeader"));
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(24, 18, 24, 18);
    hLayout->setSpacing(16);

    auto* iconLabel = new QLabel(header);
    iconLabel->setFixedSize(48, 48);
    iconLabel->setPixmap(QIcon::fromTheme(iconKey, QIcon::fromTheme(QStringLiteral("applications-system"))).pixmap(48, 48));
    hLayout->addWidget(iconLabel);

    auto* textBox = new QVBoxLayout();
    textBox->setSpacing(4);
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

    outer->addWidget(makePageHeader(this, QStringLiteral("preferences-desktop-locale"), m_headerTitle, m_headerSubtitle));

    auto* content = new QVBoxLayout();
    content->setContentsMargins(24, 16, 24, 16);
    m_langWidget = new LanguageSelectionWidget(this);
    content->addWidget(m_langWidget);
    outer->addLayout(content, 1);

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
    if (m_langWidget)
        m_langWidget->retranslate();
}
