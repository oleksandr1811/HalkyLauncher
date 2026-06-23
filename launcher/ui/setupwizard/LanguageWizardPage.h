#pragma once

#include "BaseWizardPage.h"

#include <QLabel>
#include <QToolButton>

class LanguageSelectionWidget;

class LanguageWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    explicit LanguageWizardPage(QWidget* parent = Q_NULLPTR);

    virtual ~LanguageWizardPage();

    bool wantsRefreshButton() override;

    void refresh() override;

    bool validatePage() override;

   protected:
    void retranslate() override;

   private:
    LanguageSelectionWidget* m_langWidget = nullptr;
    QLabel* m_headerTitle = nullptr;
    QLabel* m_headerSubtitle = nullptr;
    QLabel* m_telemetryLabel = nullptr;
};
