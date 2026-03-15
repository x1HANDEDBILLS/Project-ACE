// LOCATION: /home/user/ACE/src/GUI/dash/settings_page1.cpp
#include "dash/settings_page1.h"

#include <QVBoxLayout>

#include "theme.h"

SettingsPage1::SettingsPage1(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(30, 30, 30, 30);

    theme_btn = new Button("THEME COLOR", this);
    theme_btn->setFixedSize(160, 40);
    connect(theme_btn, &QPushButton::clicked, this, &SettingsPage1::setup_theme_popout);

    main_layout->addWidget(theme_btn, 0, Qt::AlignLeft | Qt::AlignTop);
    main_layout->addStretch();
}

void SettingsPage1::setup_theme_popout()
{
    ThemeManager::instance().next();
}

void SettingsPage1::refresh_theme()
{
    if (theme_btn) theme_btn->update();
}
