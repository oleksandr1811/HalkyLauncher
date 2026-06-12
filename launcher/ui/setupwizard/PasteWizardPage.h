#pragma once

#include "BaseWizardPage.h"

#include <QEvent>
#include <QLabel>
#include <QRadioButton>

class QFrame;

class PasteWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    explicit PasteWizardPage(QWidget* parent = nullptr);
    ~PasteWizardPage() override = default;

    void initializePage() override;
    bool validatePage() override;
    bool eventFilter(QObject* obj, QEvent* event) override;

   protected:
    void retranslate() override;

   private:
    QLabel* m_headerTitle = nullptr;
    QLabel* m_headerSubtitle = nullptr;
    QLabel* m_desc = nullptr;
    QRadioButton* m_defaultRadio = nullptr;
    QLabel* m_defaultDesc = nullptr;
    QRadioButton* m_previousRadio = nullptr;
    QLabel* m_previousDesc = nullptr;
    QFrame* m_defaultCard = nullptr;
    QFrame* m_previousCard = nullptr;
};
