#include "LoginWizardPage.h"

#include "Application.h"
#include "minecraft/auth/AccountList.h"
#include "ui/dialogs/ChooseOfflineNameDialog.h"
#include "ui/dialogs/CustomLoginDialog.h"
#include "ui/dialogs/ElybyLoginDialog.h"
#include "ui/dialogs/MSALoginDialog.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>

LoginWizardPage::LoginWizardPage(QWidget* parent) : BaseWizardPage(parent)
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
        QIcon::fromTheme(QStringLiteral("user-identity"), QIcon::fromTheme(QStringLiteral("preferences-system-users"))).pixmap(48, 48));
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

    // Account type buttons as cards
    auto* content = new QVBoxLayout();
    content->setContentsMargins(24, 20, 24, 20);
    content->setSpacing(10);

    auto makeLoginBtn = [&](const QString& iconName) -> QPushButton* {
        auto* btn = new QPushButton(this);
        btn->setObjectName(QStringLiteral("loginBtn"));
        btn->setCursor(Qt::PointingHandCursor);
        if (!iconName.isEmpty())
            btn->setIcon(QIcon::fromTheme(iconName));
        btn->setIconSize(QSize(24, 24));
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setFixedHeight(56);
        return btn;
    };

    m_msaBtn = makeLoginBtn(QStringLiteral("applications-internet"));
    m_elyBtn = makeLoginBtn(QStringLiteral("user-online"));
    m_offlineBtn = makeLoginBtn(QStringLiteral("user-offline"));
    m_customBtn = makeLoginBtn(QStringLiteral("preferences-system-network"));

    content->addWidget(m_msaBtn);
    content->addWidget(m_elyBtn);
    content->addWidget(m_offlineBtn);
    content->addWidget(m_customBtn);
    content->addStretch(1);
    outer->addLayout(content, 1);

    connect(m_msaBtn, &QPushButton::clicked, this, [this]() {
        wizard()->hide();
        addAccount(MSALoginDialog::newAccount(nullptr));
        wizard()->show();
    });
    connect(m_elyBtn, &QPushButton::clicked, this, [this]() {
        wizard()->hide();
        addAccount(ElybyLoginDialog::newAccount(nullptr));
        wizard()->show();
    });
    connect(m_offlineBtn, &QPushButton::clicked, this, [this]() {
        wizard()->hide();
        ChooseOfflineNameDialog dialog(tr("Please enter your desired username to add your offline account."), this);
        if (dialog.exec() == QDialog::Accepted)
            addAccount(MinecraftAccount::createOffline(dialog.getUsername()));
        wizard()->show();
    });
    connect(m_customBtn, &QPushButton::clicked, this, [this]() {
        wizard()->hide();
        addAccount(CustomLoginDialog::newAccount(nullptr, tr("Please enter authentication server URL, your username and password.")));
        wizard()->show();
    });

    retranslate();
}

void LoginWizardPage::addAccount(MinecraftAccountPtr account)
{
    if (!account)
        return;
    APPLICATION->accounts()->addAccount(account);
    APPLICATION->accounts()->setDefaultAccount(account);
    if (wizard()->currentId() == wizard()->pageIds().last())
        wizard()->accept();
    else
        wizard()->next();
}

void LoginWizardPage::initializePage() {}

bool LoginWizardPage::validatePage()
{
    return true;
}

void LoginWizardPage::retranslate()
{
    setTitle(tr("Account"));
    setSubTitle({});
    if (m_headerTitle)
        m_headerTitle->setText(tr("Add Account"));
    if (m_headerSubtitle)
        m_headerSubtitle->setText(tr("Sign in to play Minecraft. You can add more accounts later in Settings."));
    if (m_msaBtn)
        m_msaBtn->setText(tr("  Microsoft account  (Minecraft: Java Edition)"));
    if (m_elyBtn)
        m_elyBtn->setText(tr("  Ely.by account"));
    if (m_offlineBtn)
        m_offlineBtn->setText(tr("  Offline account  (no authentication)"));
    if (m_customBtn)
        m_customBtn->setText(tr("  Custom auth server"));
}
