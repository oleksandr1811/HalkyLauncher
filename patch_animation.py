import re

with open("launcher/ui/widgets/HalkyNavBar.cpp", "r", encoding="utf-8") as f:
    content = f.read()

# Add include
content = content.replace("#include <QPolygonF>", "#include <QPolygonF>\n#include <QVariantAnimation>")


# Add to constructor
constructor_old = """HalkyNavBar::HalkyNavBar(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("halkyNavBar"));
    setFixedWidth(COLLAPSED_W);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    buildLayout();
}"""

constructor_new = """HalkyNavBar::HalkyNavBar(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("halkyNavBar"));
    setFixedWidth(COLLAPSED_W);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    buildLayout();

    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(250);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        this->setFixedWidth(value.toInt());
    });
}"""

content = content.replace(constructor_old, constructor_new)


# Modify toggleExpand
toggle_old = """void HalkyNavBar::toggleExpand()
{
    m_expanded = !m_expanded;
    const auto style = m_expanded ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;

    // Header: show/hide launcher name label
    m_launcherLabel->setVisible(m_expanded);
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

    setFixedWidth(m_expanded ? EXPANDED_W : COLLAPSED_W);
}"""

toggle_new = """void HalkyNavBar::toggleExpand()
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

content = content.replace(toggle_old, toggle_new)

with open("launcher/ui/widgets/HalkyNavBar.cpp", "w", encoding="utf-8") as f:
    f.write(content)
