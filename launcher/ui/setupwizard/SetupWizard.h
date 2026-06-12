#pragma once

#include <QPixmap>
#include <QWizard>

class BaseWizardPage;

class SetupWizard : public QWizard {
    Q_OBJECT

   public:
    explicit SetupWizard(QWidget* parent = nullptr);
    ~SetupWizard() override;

    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

    BaseWizardPage* getBasePage(int id);
    BaseWizardPage* getCurrentBasePage();

   private slots:
    void pageChanged(int id);

   private:
    void retranslate();
    QPixmap generateSidebarPixmap(int currentIndex);

    static constexpr int SIDEBAR_W = 220;
};
