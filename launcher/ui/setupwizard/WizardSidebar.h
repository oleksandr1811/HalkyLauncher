#pragma once

#include <QList>
#include <QString>
#include <QWidget>

class WizardSidebar : public QWidget {
    Q_OBJECT
   public:
    struct Step {
        QString name;
    };

    explicit WizardSidebar(QWidget* parent = nullptr);

    void setSteps(const QList<Step>& steps);
    void setCurrentStep(int index);
    void retranslate();

   protected:
    void paintEvent(QPaintEvent* e) override;

   private:
    QList<Step> m_steps;
    int m_currentStep = 0;

    static constexpr int W = 220;
    static constexpr int STEP_H = 44;
    static constexpr int HEADER_H = 96;
};
