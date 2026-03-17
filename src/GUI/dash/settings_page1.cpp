// Location: src/GUI/dash/settings_page1.cpp
#include "dash/settings_page1.h"
#include "theme/theme.h"
#include <QVBoxLayout>
SettingsPage1::SettingsPage1(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 20);
    theme_btn = new Button("ROTARY THEME", this);
    theme_btn->setFixedSize(220, 60);
    connect(theme_btn, &QPushButton::clicked, this, &SettingsPage1::setup_theme_popout);
    main_layout->addWidget(theme_btn, 0, Qt::AlignCenter);
    main_layout->addStretch();
}
void SettingsPage1::setup_theme_popout() {
    ThemeManager::instance().next();
    // The parent SettingsPanel will catch the themeChanged signal (if connected)
    // or trigger refresh_theme() via the loop we wrote.
}
void SettingsPage1::refresh_theme() {
    if (theme_btn) theme_btn->update();
}
