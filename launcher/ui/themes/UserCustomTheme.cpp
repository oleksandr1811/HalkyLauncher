// SPDX-License-Identifier: GPL-3.0-only
#include "UserCustomTheme.h"
#include <QObject>
#include <Application.h>
#include "settings/SettingsObject.h"

QString UserCustomTheme::id() { return "user-custom"; }

QString UserCustomTheme::name() { return QObject::tr("Custom (Settings)"); }

QString UserCustomTheme::tooltip() { return ""; }

bool UserCustomTheme::hasStyleSheet() { return true; }

QString UserCustomTheme::appStyleSheet() {
    return "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }";
}

double UserCustomTheme::fadeAmount() { return 0.5; }

QColor UserCustomTheme::fadeColor() { return QColor(49, 49, 49); }

QPalette UserCustomTheme::colorScheme() {
    QPalette pal;
    auto settings = APPLICATION->settings();
    pal.setColor(QPalette::Window, QColor(settings->get("CustomTheme_Window").toString()));
    pal.setColor(QPalette::WindowText, QColor(settings->get("CustomTheme_WindowText").toString()));
    pal.setColor(QPalette::Base, QColor(settings->get("CustomTheme_Base").toString()));
    pal.setColor(QPalette::AlternateBase, QColor(settings->get("CustomTheme_AlternateBase").toString()));
    pal.setColor(QPalette::ToolTipBase, QColor(settings->get("CustomTheme_ToolTipBase").toString()));
    pal.setColor(QPalette::ToolTipText, QColor(settings->get("CustomTheme_ToolTipText").toString()));
    pal.setColor(QPalette::Text, QColor(settings->get("CustomTheme_Text").toString()));
    pal.setColor(QPalette::Button, QColor(settings->get("CustomTheme_Button").toString()));
    pal.setColor(QPalette::ButtonText, QColor(settings->get("CustomTheme_ButtonText").toString()));
    pal.setColor(QPalette::BrightText, QColor(settings->get("CustomTheme_BrightText").toString()));
    pal.setColor(QPalette::Link, QColor(settings->get("CustomTheme_Link").toString()));
    pal.setColor(QPalette::Highlight, QColor(settings->get("CustomTheme_Highlight").toString()));
    pal.setColor(QPalette::HighlightedText, QColor(settings->get("CustomTheme_HighlightedText").toString()));
    pal.setColor(QPalette::PlaceholderText, QColor(settings->get("CustomTheme_PlaceholderText").toString()));
    return ITheme::fadeInactive(pal, fadeAmount(), fadeColor());
}
