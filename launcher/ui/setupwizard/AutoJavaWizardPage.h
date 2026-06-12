#pragma once

#include "BaseWizardPage.h"

#include <QEvent>
#include <QLabel>
#include <QRadioButton>

class QFrame;

class AutoJavaWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    explicit AutoJavaWizardPage(QWidget* parent = nullptr);
    ~AutoJavaWizardPage() override = default;

    void initializePage() override;
    bool validatePage() override;
    bool eventFilter(QObject* obj, QEvent* event) override;

   protected:
    void retranslate() override;

   private:
    QLabel* m_headerTitle = nullptr;
    QLabel* m_headerSubtitle = nullptr;
    QLabel* m_desc = nullptr;
    QRadioButton* m_enableRadio = nullptr;
    QLabel* m_enableDesc = nullptr;
    QRadioButton* m_disableRadio = nullptr;
    QLabel* m_disableDesc = nullptr;
    QFrame* m_enableCard = nullptr;
    QFrame* m_disableCard = nullptr;
};
