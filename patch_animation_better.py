import re

with open("launcher/ui/widgets/HalkyNavBar.cpp", "r", encoding="utf-8") as f:
    content = f.read()

# Modify toggleExpand for better animation
toggle_old = """void HalkyNavBar::toggleExpand()
{
    m_expanded = !m_expanded;

    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    
    m_animation->setStartValue(this->width());
    m_animation->setEndValue(m_expanded ? EXPANDED_W : COLLAPSED_W);

    const auto style = m_expanded ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;

    // When expanding, show texts immediately to let layout adjust inside, clipping will hide it gracefully
    m_launcherLabel->setVisible(true); // Ensure it's visible during animation
    
    m_toggleBtn->setIcon(paintedIcon(m_expanded ? QStringLiteral("collapse") : QStringLiteral("expand")));
    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));

    // Nav items
    for (auto& item : m_navItems) {
        item.btn->setToolButtonStyle(style);
        item.btn->setToolTip(m_expanded ? QString() : item.defaultText);
    }

    // Utility buttons
    auto applyUtil = [&](QToolButton* btn, const QString& label) {
        btn->setToolButtonStyle(style);
        btn->setToolTip(m_expanded ? QString() : label);
    };
    applyUtil(m_addBtn, tr("Add Instance"));
    applyUtil(m_accountsBtn, tr("Accounts"));
    applyUtil(m_foldersBtn, tr("Folders"));
    applyUtil(m_settingsBtn, tr("Settings"));
    applyUtil(m_helpBtn, tr("Help"));

    if (!m_expanded) {
        // Hide launcher label immediately to avoid text overflow when contracting
        m_launcherLabel->setVisible(false);
    }

    m_animation->start();
}"""

toggle_new = """void HalkyNavBar::toggleExpand()
{
    m_expanded = !m_expanded;

    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    
    m_animation->setStartValue(this->width());
    m_animation->setEndValue(m_expanded ? EXPANDED_W : COLLAPSED_W);

    // Keep ToolButtonTextBesideIcon during the whole expanding/collapsing phase
    // purely for a better clipping effect. We'll set it to IconOnly when collapse is actually finished.
    const auto style = Qt::ToolButtonTextBesideIcon;

    m_launcherLabel->setVisible(true); // Keep label visible during both animations to allow clipping
    
    m_toggleBtn->setIcon(paintedIcon(m_expanded ? QStringLiteral("collapse") : QStringLiteral("expand")));
    m_toggleBtn->setToolTip(m_expanded ? tr("Collapse navigation") : tr("Expand navigation"));

    // Nav items
    for (auto& item : m_navItems) {
        item.btn->setToolButtonStyle(style);
        item.btn->setToolTip(m_expanded ? QString() : item.defaultText);
    }

    // Utility buttons
    auto applyUtil = [&](QToolButton* btn, const QString& label) {
        btn->setToolButtonStyle(style);
        btn->setToolTip(m_expanded ? QString() : label);
    };
    applyUtil(m_addBtn, tr("Add Instance"));
    applyUtil(m_accountsBtn, tr("Accounts"));
    applyUtil(m_foldersBtn, tr("Folders"));
    applyUtil(m_settingsBtn, tr("Settings"));
    applyUtil(m_helpBtn, tr("Help"));

    m_animation->start();
}"""

content = content.replace(toggle_old, toggle_new)

# Modify constructor to connect finished signal
constructor_old = """    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(250);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        this->setFixedWidth(value.toInt());
    });
}"""

constructor_new = """    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(250);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        this->setFixedWidth(value.toInt());
    });
    connect(m_animation, &QVariantAnimation::finished, this, [this]() {
        if (!m_expanded) {
            m_launcherLabel->setVisible(false);
            const auto style = Qt::ToolButtonIconOnly;
            for (auto& item : m_navItems) {
                item.btn->setToolButtonStyle(style);
            }
            m_addBtn->setToolButtonStyle(style);
            m_accountsBtn->setToolButtonStyle(style);
            m_foldersBtn->setToolButtonStyle(style);
            m_settingsBtn->setToolButtonStyle(style);
            m_helpBtn->setToolButtonStyle(style);
        }
    });
}"""

content = content.replace(constructor_old, constructor_new)

with open("launcher/ui/widgets/HalkyNavBar.cpp", "w", encoding="utf-8") as f:
    f.write(content)

