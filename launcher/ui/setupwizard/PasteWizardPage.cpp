#include "PasteWizardPage.h"

#include "Application.h"
#include "net/PasteUpload.h"
#include "settings/SettingsObject.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QVBoxLayout>

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

    card->installEventFilter(parent);
    return card;
}

PasteWizardPage::PasteWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Header
    auto* header = new QFrame(this);
    header->setObjectName(QStringLiteral("wizardPageHeader"));
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(28, 20, 28, 20);
    hLayout->setSpacing(20);
    auto* iconLabel = new QLabel(header);
    iconLabel->setFixedSize(52, 52);
    iconLabel->setPixmap(QIcon::fromTheme(QStringLiteral("proxy")).pixmap(52, 52));
    hLayout->addWidget(iconLabel);
    auto* textBox = new QVBoxLayout();
    textBox->setSpacing(6);
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

    m_defaultCard = makeOptionCard(this, m_defaultRadio, m_defaultDesc);
    m_defaultRadio->setChecked(true);
    content->addWidget(m_defaultCard);

    m_previousCard = makeOptionCard(this, m_previousRadio, m_previousDesc);
    content->addWidget(m_previousCard);

    content->addStretch(1);

    auto* group = new QButtonGroup(this);
    group->addButton(m_defaultRadio);
    group->addButton(m_previousRadio);

    outer->addLayout(content, 1);
    retranslate();
}

void PasteWizardPage::initializePage() {}

bool PasteWizardPage::validatePage()
{
    auto* s = APPLICATION->settings();
    const QString prevPasteURL = s->get("PastebinURL").toString();
    s->reset("PastebinURL");
    if (m_previousRadio->isChecked()) {
        const bool usingDefaultBase =
            prevPasteURL == PasteUpload::PasteTypes.at(PasteUpload::PasteType::NullPointer).defaultBase;
        s->set("PastebinType", PasteUpload::PasteType::NullPointer);
        if (!usingDefaultBase)
            s->set("PastebinCustomAPIBase", prevPasteURL);
    }
    return true;
}

bool PasteWizardPage::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        if (obj == m_defaultCard)
            m_defaultRadio->setChecked(true);
        else if (obj == m_previousCard)
            m_previousRadio->setChecked(true);
    }
    return BaseWizardPage::eventFilter(obj, event);
}

void PasteWizardPage::retranslate()
{
    setTitle(tr("Paste Service"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Paste Service"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("The default log upload service has changed."));
    if (m_desc)
        m_desc->setText(
            tr("The default paste service has changed to mclo.gs. Choose what to do with your existing paste settings."));
    if (m_defaultRadio)
        m_defaultRadio->setText(tr("Use new default service (mclo.gs)"));
    if (m_defaultDesc)
        m_defaultDesc->setText(tr("Recommended — switch to the new, faster mclo.gs service."));
    if (m_previousRadio)
        m_previousRadio->setText(tr("Keep previous settings"));
    if (m_previousDesc)
        m_previousDesc->setText(tr("Your existing custom paste service URL will be preserved."));
}
