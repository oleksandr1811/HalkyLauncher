#include "JavaWizardPage.h"

#include <BuildConfig.h>
#include "Application.h"
#include "JavaCommon.h"
#include "settings/SettingsObject.h"
#include "ui/widgets/JavaWizardWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

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

JavaWizardPage::JavaWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    setupUi();
}

void JavaWizardPage::setupUi()
{
    setObjectName(QStringLiteral("javaPage"));

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    outer->addWidget(makePageHeader(this, QStringLiteral("application-x-java"), m_headerTitle, m_headerSubtitle));

    // Scroll area so Java widget fits on small screens
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_java_widget = new JavaWizardWidget(scroll);
    scroll->setWidget(m_java_widget);

    auto* content = new QVBoxLayout();
    content->setContentsMargins(16, 12, 16, 12);
    content->addWidget(scroll);
    outer->addLayout(content, 1);

    retranslate();
}

void JavaWizardPage::refresh()
{
    m_java_widget->refresh();
}

void JavaWizardPage::initializePage()
{
    m_java_widget->initialize();
}

bool JavaWizardPage::wantsRefreshButton()
{
    return true;
}

bool JavaWizardPage::validatePage()
{
    auto* settings = APPLICATION->settings();
    auto result = m_java_widget->validate();
    settings->set("AutomaticJavaSwitch", m_java_widget->autoDetectJava());
    settings->set("AutomaticJavaDownload", m_java_widget->autoDownloadJava());
    settings->set("UserAskedAboutAutomaticJavaDownload", true);
    switch (result) {
        default:
        case JavaWizardWidget::ValidationStatus::Bad:
            return false;
        case JavaWizardWidget::ValidationStatus::AllOK:
            settings->set("JavaPath", m_java_widget->javaPath());
            [[fallthrough]];
        case JavaWizardWidget::ValidationStatus::JavaBad:
            settings->set("MinMemAlloc", m_java_widget->minHeapSize());
            settings->set("MaxMemAlloc", m_java_widget->maxHeapSize());
            if (m_java_widget->permGenEnabled())
                settings->set("PermGen", m_java_widget->permGenSize());
            else
                settings->reset("PermGen");
            return true;
    }
}

void JavaWizardPage::retranslate()
{
    setTitle(tr("Java"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Java"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(
            tr("Choose Java memory allocation and whether %1 should manage Java automatically.").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    if (m_java_widget)
        m_java_widget->retranslate();
}
