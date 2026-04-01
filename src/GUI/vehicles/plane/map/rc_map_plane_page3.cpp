#include "Core/app_state.h"
#include "profiles/profile_manager.h"
#include "rc_map_plane_page3.h"
#include "theme/theme.h"
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RCMapPlanePage3::RCMapPlanePage3(QWidget *parent) : QWidget(parent) {
    auto *main = new QVBoxLayout(this);
    auto *grid = new QGridLayout();
    grid->setSpacing(8);

    for (int i = 0; i < 16; i++) {
        QFrame *f = new QFrame();
        f->setStyleSheet("background: #0f0f15; border: 1px solid #222; border-radius: 6px;");
        auto *l = new QVBoxLayout(f);
        l->setContentsMargins(5, 5, 5, 5);
        l->setSpacing(4);

        QLabel *name = new QLabel(QString("CH %1").arg(i + 1));
        name->setStyleSheet("color: #555; font-size: 11px; font-weight: bold; font-family: 'Consolas';");
        name->setAlignment(Qt::AlignCenter);

        rev_buttons[i] = new QPushButton();
        rev_buttons[i]->setFixedHeight(35);
        connect(rev_buttons[i], &QPushButton::clicked, [this, i]() {
            AppState::instance().inverted[i] = !AppState::instance().inverted[i];
            ProfileManager::instance().save(); // FIXED: using .save() instead of non-existent .save_current_profile()
            refresh_ui();
        });

        thr_buttons[i] = new QPushButton("THR");
        thr_buttons[i]->setFixedHeight(35);
        connect(thr_buttons[i], &QPushButton::clicked, [this, i]() {
            AppState::instance().throttle_mode[i] = !AppState::instance().throttle_mode[i];
            ProfileManager::instance().save(); // FIXED: using .save()
            refresh_ui();
        });

        l->addWidget(name);
        l->addWidget(rev_buttons[i]);
        l->addWidget(thr_buttons[i]);
        grid->addWidget(f, i / 4, i % 4);
    }
    main->addStretch();
    main->addLayout(grid);
    main->addStretch();

    connect(&ProfileManager::instance(), &ProfileManager::profileLoaded, this, &RCMapPlanePage3::refresh_ui);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &RCMapPlanePage3::refresh_ui);
    refresh_ui();
}

void RCMapPlanePage3::refresh_ui() {
    auto &state = AppState::instance();
    QString accent = ThemeManager::instance().active().hex;
    for (int i = 0; i < 16; i++) {
        bool inv = state.inverted[i];
        rev_buttons[i]->setText(inv ? "REV" : "NOR");
        if (inv) {
            rev_buttons[i]->setStyleSheet(QString("background: %1; color: black; font-weight: bold; border: none; font-size: 14px; font-family: 'Consolas';").arg(accent));
        } else {
            rev_buttons[i]->setStyleSheet("background: #222; color: #777; border: 1px solid #333; font-size: 14px; font-family: 'Consolas';");
        }

        bool t_mode = state.throttle_mode[i];
        if (t_mode) {
            thr_buttons[i]->setStyleSheet("background: #ffb000; color: black; font-weight: bold; border: none; font-size: 14px; font-family: 'Consolas';");
        } else {
            thr_buttons[i]->setStyleSheet("background: #1a1a1f; color: #444; border: 1px solid #222; font-size: 14px; font-family: 'Consolas';");
        }
    }
}
