#pragma once

#include "BaseWizardPage.h"
#include "minecraft/auth/MinecraftAccount.h"

#include <QLabel>
#include <QPushButton>

class LoginWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    explicit LoginWizardPage(QWidget* parent = nullptr);
    ~LoginWizardPage() override = default;

    void initializePage() override;
    bool validatePage() override;

   protected:
    void retranslate() override;

   private:
    void addAccount(MinecraftAccountPtr account);

    QLabel* m_headerTitle = nullptr;
    QLabel* m_headerSubtitle = nullptr;
    QPushButton* m_msaBtn = nullptr;
    QPushButton* m_elyBtn = nullptr;
    QPushButton* m_offlineBtn = nullptr;
    QPushButton* m_customBtn = nullptr;
};
