// LOCATION: /home/user/ACE/src/GUI/rc_type/rc_type_selector.cpp
#include "rc_type/rc_type_selector.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <iostream>

#include "Core/app_state.h"
#include "input/input_manage.h"
#include "logic/logic.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"

RCTypeSelector::RCTypeSelector(QWidget *parent) : QWidget(parent)
{
    // FIX: Set a minimum size so it doesn't disappear in layouts
    setMinimumSize(600, 100);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignCenter);

    group = new QButtonGroup(this);
    QStringList types = {"PLANE", "DRONE", "HELI", "CAR"};
    auto theme = ThemeManager::instance().active();

    for (int i = 0; i < types.size(); ++i) {
        QPushButton *btn = new QPushButton(types[i], this);
        btn->setCheckable(true);
        btn->setFixedSize(130, 60);

        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: %2; border: 1px solid %3; "
                    "font-family: 'Consolas'; font-weight: bold; font-size: 14px; }"
                    "QPushButton:checked { background-color: %3; color: #000; border: 2px solid "
                    "white; }")
                .arg(theme.bg_dark.name())
                .arg(theme.secondary.name())
                .arg(theme.primary.name()));

        layout->addWidget(btn);
        group->addButton(btn, i);
    }

    connect(group, &QButtonGroup::idClicked, [this, types](int id) {
        InputManager::instance().setVehicleType(id);
        AppState::instance().rc_type = types[id];
        emit typeChanged(id);
    });

    // The Master Handshake: Ensure the 2000Hz engine unlocks after GUI is visible
    QTimer::singleShot(500, [this]() {
        refresh_selection();
        Logic::unlock_backend();
        std::cout << "\033[1;35m[GUI] Selector Visible & Backend Unlocked\033[0m" << std::endl;
    });
}

void RCTypeSelector::refresh_selection()
{
    QString current = AppState::instance().rc_type;
    QStringList types = {"PLANE", "DRONE", "HELI", "CAR"};
    int idx = types.indexOf(current);

    if (idx != -1 && group->button(idx)) {
        group->blockSignals(true);  // Don't trigger loops
        group->button(idx)->setChecked(true);
        InputManager::instance().setVehicleType(idx);
        group->blockSignals(false);
    }
}
