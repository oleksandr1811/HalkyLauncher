#include "AutoJavaWizardPage.h"

#include <BuildConfig.h>
#include "Application.h"
#include "settings/SettingsObject.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QVBoxLayout>

// Helper: builds a styled "option card" frame with a radio + title + description
static QFrame* makeOptionCard(QWidget* parent, QRadioButton*& radioOut, QLabel*& descOut)
{
    auto* card = new QFrame(parent);
    card->setObjectName(QStringLiteral("optionCard"));
    card->setCursor(Qt::PointingHandCursor);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(6);

    radioOut = new QRadioButton(card);
    radioOut->setObjectName(QStringLiteral("cardRadio"));

    descOut = new QLabel(card);
    descOut->setObjectName(QStringLiteral("cardDesc"));
    descOut->setWordWrap(true);
    descOut->setContentsMargins(26, 0, 0, 0);

    layout->addWidget(radioOut);
    layout->addWidget(descOut);

    // Clicking anywhere on the card selects the radio
    card->installEventFilter(parent);

    return card;
}

AutoJavaWizardPage::AutoJavaWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Header
    auto* header = new QFrame(this);
    header->setObjectName(QStringLiteral("wizardPageHeader"));
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(24, 18, 24, 18);
    hLayout->setSpacing(16);
    auto* iconLabel = new QLabel(header);
    iconLabel->setFixedSize(48, 48);
    iconLabel->setPixmap(
        QIcon::fromTheme(QStringLiteral("system-software-update"), QIcon::fromTheme(QStringLiteral("applications-system"))).pixmap(48, 48));
    hLayout->addWidget(iconLabel);
    auto* textBox = new QVBoxLayout();
    textBox->setSpacing(4);
    m_headerTitle = new QLabel(header);
    m_headerTitle->setObjectName(QStringLiteral("wizardPageTitle"));
    m_headerSubtitle = new QLabel(header);
    m_headerSubtitle->setObjectName(QStringLiteral("wizardPageSubtitle"));
    m_headerSubtitle->setWordWrap(true);
    textBox->addWidget(m_headerTitle);
    textBox->addWidget(m_headerSubtitle);
    hLayout->addLayout(textBox, 1);
    outer->addWidget(header);

    // Content
    auto* content = new QVBoxLayout();
    content->setContentsMargins(24, 20, 24, 20);
    content->setSpacing(12);

    m_desc = new QLabel(this);
    m_desc->setWordWrap(true);
    m_desc->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 12px;"));
    content->addWidget(m_desc);
    content->addSpacing(8);

    m_enableCard = makeOptionCard(this, m_enableRadio, m_enableDesc);
    m_enableRadio->setChecked(true);
    content->addWidget(m_enableCard);

    m_disableCard = makeOptionCard(this, m_disableRadio, m_disableDesc);
    content->addWidget(m_disableCard);

    content->addStretch(1);

    // Mutual exclusion
    auto* group = new QButtonGroup(this);
    group->addButton(m_enableRadio);
    group->addButton(m_disableRadio);

    outer->addLayout(content, 1);
    retranslate();
}

void AutoJavaWizardPage::initializePage() {}

bool AutoJavaWizardPage::validatePage()
{
    auto* s = APPLICATION->settings();
    if (!m_disableRadio->isChecked()) {
        s->set("AutomaticJavaSwitch", true);
        s->set("AutomaticJavaDownload", true);
    }
    s->set("UserAskedAboutAutomaticJavaDownload", true);
    return true;
}

bool AutoJavaWizardPage::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        if (obj == m_enableCard)
            m_enableRadio->setChecked(true);
        else if (obj == m_disableCard)
            m_disableRadio->setChecked(true);
    }
    return BaseWizardPage::eventFilter(obj, event);
}

void AutoJavaWizardPage::retranslate()
{
    setTitle(tr("Automatic Java"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Automatic Java"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("Automatically download the correct Java version for each Minecraft version."));
    if (m_desc)
        m_desc->setText(tr("A new feature was added that can automatically download and switch to the correct Java version for each instance. "
                           "Would you like to enable it?"));
    if (m_enableRadio)
        m_enableRadio->setText(tr("Enable automatic Java download"));
    if (m_enableDesc)
        m_enableDesc->setText(tr("Recommended — %1 will download and manage Java automatically.").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    if (m_disableRadio)
        m_disableRadio->setText(tr("Keep manual Java settings"));
    if (m_disableDesc)
        m_disableDesc->setText(tr("You will manage Java installation and selection yourself."));
}
