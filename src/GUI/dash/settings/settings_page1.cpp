#include "settings_page1.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"
#include <QVBoxLayout>
#include <QApplication>

SettingsPage1::SettingsPage1(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 20);
    main_layout->setSpacing(20);

    // 1. Theme Cycle Button
    theme_btn = new Button("ROTARY THEME", this);
    theme_btn->setFixedSize(220, 60);
    connect(theme_btn, &QPushButton::clicked, this, &SettingsPage1::setup_theme_popout);
    main_layout->addWidget(theme_btn, 0, Qt::AlignCenter);

    // 2. Safe Exit Button
    exit_btn = new Button("EXIT APPLICATION", this);
    exit_btn->setFixedSize(220, 60);
    
    // Applying a "Warning" style for the exit button
    exit_btn->setStyleSheet("QPushButton { background: #331111; color: #ff4444; border: 1px solid #552222; font-weight: bold; } "
                            "QPushButton:pressed { background: #ff4444; color: black; }");

    connect(exit_btn, &QPushButton::clicked, [this]() {
        // Safe Shutdown Sequence:
        // A. Save the current state to the active profile
        ProfileManager::instance().save();
        
        // B. Quit the Qt Event Loop
        QApplication::quit();
    });

    main_layout->addWidget(exit_btn, 0, Qt::AlignCenter);
    main_layout->addStretch();
}

void SettingsPage1::setup_theme_popout() {
    ThemeManager::instance().next();
}

void SettingsPage1::refresh_theme() {
    if (theme_btn) theme_btn->update();
    // The Exit button has a hardcoded 'danger' style, 
    // but we can update its border to match the theme here if preferred.
}
