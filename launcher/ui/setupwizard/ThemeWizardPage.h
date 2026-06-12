// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Tayou <git@tayou.org>
 */
#pragma once

#include <ui/widgets/AppearanceWidget.h>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include "BaseWizardPage.h"

class ThemeWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    ThemeWizardPage(QWidget* parent = nullptr) : BaseWizardPage(parent)
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
            QIcon::fromTheme(QStringLiteral("preferences-desktop-theme"),
                             QIcon::fromTheme(QStringLiteral("applications-graphics")))
                .pixmap(48, 48));
        hLayout->addWidget(iconLabel);
        auto* textBox = new QVBoxLayout();
        textBox->setSpacing(4);
        m_title = new QLabel(header);
        m_title->setObjectName(QStringLiteral("wizardPageTitle"));
        m_subtitle = new QLabel(header);
        m_subtitle->setObjectName(QStringLiteral("wizardPageSubtitle"));
        m_subtitle->setWordWrap(true);
        textBox->addWidget(m_title);
        textBox->addWidget(m_subtitle);
        hLayout->addLayout(textBox, 1);
        outer->addWidget(header);

        // Content
        auto* content = new QVBoxLayout();
        content->setContentsMargins(16, 12, 16, 12);
        content->addWidget(&m_widget);
        content->addStretch(1);
        outer->addLayout(content, 1);

        retranslate();
    }

    bool validatePage() override { return true; }

    void retranslate() override
    {
        setTitle(tr("Appearance"));
        setSubTitle({});
        if (m_title)
            m_title->setText(tr("Appearance"));
        if (m_subtitle)
            m_subtitle->setText(tr("Choose a theme and icon set that suits you."));
        m_widget.retranslateUi();
    }

   private:
    AppearanceWidget m_widget{ true };
    QLabel* m_title = nullptr;
    QLabel* m_subtitle = nullptr;
};
