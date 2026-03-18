// Location: src/GUI/dash/dashboard_panel.cpp
#include "dashboard_panel.h"
#include "main_car_dash.h"
#include "main_drone_dash.h"
#include "main_heli_dash.h"
#include "main_plane_dash.h"
#include "theme/theme.h"
#include "input_tune_car_panel.h"
#include "rc_map_car_panel.h"
#include "rc_tune_car_panel.h"
#include "input_tune_drone_panel.h"
#include "rc_map_drone_panel.h"
#include "rc_tune_drone_panel.h"
#include "input_tune_heli_panel.h"
#include "rc_map_heli_panel.h"
#include "rc_tune_heli_panel.h"
#include "input_tune_plane_panel.h"
#include "rc_map_plane_panel.h"
#include "rc_mix_plane_panel.h"
#include "rc_trim_plane_panel.h"
#include "rc_tune_plane_panel.h"
#include "widgets/button.h"
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QVBoxLayout>
DashboardPanel::DashboardPanel(QWidget *parent) : QWidget(parent) {
    main_stack = new QStackedWidget(this);
    main_stack->addWidget(new MainPlaneDash(this));
    main_stack->addWidget(new MainDroneDash(this));
    main_stack->addWidget(new MainHeliDash(this));
    main_stack->addWidget(new MainCarDash(this));
    main_stack->lower();
    main_stack->show();
    settings_btn = new Button("⚙", this);
    settings_btn->setFixedSize(74, 74);
    connect(settings_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_sidebar);
    shield = new QFrame(this);
    shield->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    shield->hide();
    sidebar = new QFrame(this);
    sidebar->setFixedWidth(sidebar_width);
    sidebar->hide();
    QVBoxLayout *side_layout = new QVBoxLayout(sidebar);
    side_layout->setContentsMargins(0, 0, 0, 0);
    button_grid = new PanelButtons(sidebar);
    side_layout->addWidget(button_grid);
    input_gui = new InputReadPanel(shield);
    settings_gui = new SettingsPanel(shield);
    rc_type_gui = new RCTypePanel(shield);
    profile_gui = new ProfilePanel(shield);
    rc_stack = new QStackedWidget(shield);
    rc_stack->addWidget(new RCTunePlanePanel(this));
    rc_stack->addWidget(new RCTuneDronePanel(this));
    rc_stack->addWidget(new RCTuneHeliPanel(this));
    rc_stack->addWidget(new RCTuneCarPanel(this));
    input_stack = new QStackedWidget(shield);
    input_stack->addWidget(new InputTunePlanePanel(this));
    input_stack->addWidget(new InputTuneDronePanel(this));
    input_stack->addWidget(new InputTuneHeliPanel(this));
    input_stack->addWidget(new InputTuneCarPanel(this));
    rc_map_stack = new QStackedWidget(shield);
    rc_map_stack->addWidget(new RCMapPlanePanel(this));
    rc_map_stack->addWidget(new RCMapDronePanel(this));
    rc_map_stack->addWidget(new RCMapHeliPanel(this));
    rc_map_stack->addWidget(new RCMapCarPanel(this));
    trim_stack = new QStackedWidget(shield);
    trim_stack->addWidget(new RCTrimPlanePanel(this));
    trim_stack->addWidget(new QWidget());
    trim_stack->addWidget(new QWidget());
    trim_stack->addWidget(new QWidget());
    rc_mix_stack = new QStackedWidget(shield);
    rc_mix_stack->addWidget(new RCMixPlanePanel(this));
    rc_mix_stack->addWidget(new QWidget());
    rc_mix_stack->addWidget(new QWidget());
    rc_mix_stack->addWidget(new QWidget());
    QList<QWidget *> panels = {input_gui,   settings_gui, rc_type_gui, profile_gui, rc_stack,
                               input_stack, rc_map_stack, trim_stack,  rc_mix_stack};
    for (auto p : panels) {
        p->setFixedSize(834, 600);
        p->hide();
    }
    anim_side = new QPropertyAnimation(sidebar, "pos", this);
    anim_side->setDuration(250);
    anim_side->setEasingCurve(QEasingCurve::OutCubic);
    connect(button_grid->back_btn, &QPushButton::clicked, this, &DashboardPanel::smart_back);
    connect(button_grid->input_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_input_gui);
    connect(button_grid->inner_settings_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_settings_gui);
    connect(button_grid->rc_type_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_rc_type_gui);
    connect(button_grid->profile_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_profile_gui);
    connect(button_grid->rc_tune_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_rc_tune);
    connect(button_grid->tune_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_input_tune);
    connect(button_grid->rc_map_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_rc_map);
    connect(button_grid->trim_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_trim);
    connect(button_grid->rc_mix_btn, &QPushButton::clicked, this, &DashboardPanel::toggle_rc_mix);
    connect(rc_type_gui->getSelector(), &RCTypeSelector::typeChanged, this, &DashboardPanel::onVehicleTypeChanged);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &DashboardPanel::refresh_theme);
    connect(anim_side, &QPropertyAnimation::finished, this, [this]() {
        if (!sidebar_open) {
            sidebar->hide();
            shield->hide();
            settings_btn->raise();
        }
    });
    refresh_theme();
}
void DashboardPanel::update_data() { this->update(); }
void DashboardPanel::refresh_theme() {
    QString accent = ThemeManager::instance().active().hex;
    sidebar->setStyleSheet(
        QString("QFrame { background-color: rgba(5, 5, 5, 220); border-left: 2px solid %1; }").arg(accent));
}
void DashboardPanel::update_panel_positions() {
    int w = width();
    int h = height();
    int tx = (((sidebar_open ? (w - sidebar_width) : w)) - 834) / 2;
    int ty = (h - 600) / 2;
    QList<QWidget *> ps = {input_gui,   settings_gui, rc_type_gui, profile_gui, rc_stack,
                           input_stack, rc_map_stack, trim_stack,  rc_mix_stack};
    for (auto p : ps) p->move(tx, ty);
}
void DashboardPanel::hide_all_guis() {
    input_gui->hide();
    settings_gui->hide();
    rc_type_gui->hide();
    profile_gui->hide();
    rc_stack->hide();
    input_stack->hide();
    rc_map_stack->hide();
    trim_stack->hide();
    rc_mix_stack->hide();
    input_open = settings_open = rc_type_open = profile_open = rc_tune_open = input_tune_open = rc_map_open =
        trim_open = rc_mix_open = false;
}
void DashboardPanel::smart_back() {
    if (rc_mix_open) {
        rc_mix_open = false;
        rc_mix_stack->hide();
    } else if (trim_open) {
        trim_open = false;
        trim_stack->hide();
    } else if (rc_map_open) {
        rc_map_open = false;
        rc_map_stack->hide();
    } else if (rc_tune_open) {
        rc_tune_open = false;
        rc_stack->hide();
    } else if (input_tune_open) {
        input_tune_open = false;
        input_stack->hide();
    } else if (input_open) {
        input_open = false;
        input_gui->hide();
    } else if (rc_type_open) {
        rc_type_open = false;
        rc_type_gui->hide();
    } else if (profile_open) {
        profile_open = false;
        profile_gui->hide();
    } else if (settings_open) {
        settings_open = false;
        settings_gui->hide();
    } else if (sidebar_open) {
        sidebar_open = false;
        anim_side->stop();
        anim_side->setEndValue(QPoint(width(), 0));
        anim_side->start();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_sidebar() {
    int w = width();
    int h = height();
    if (!sidebar_open) {
        sidebar_open = true;
        shield->setGeometry(0, 0, w, h);
        shield->show();
        sidebar->show();
        shield->raise();
        settings_btn->raise();
        sidebar->raise();
        anim_side->stop();
        anim_side->setStartValue(QPoint(w, 0));
        anim_side->setEndValue(QPoint(w - sidebar_width, 0));
        anim_side->start();
    } else {
        smart_back();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_input_gui() {
    if (!input_open) {
        hide_all_guis();
        input_open = true;
        input_gui->show();
    } else {
        input_open = false;
        input_gui->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_settings_gui() {
    if (!settings_open) {
        hide_all_guis();
        settings_open = true;
        settings_gui->show();
    } else {
        settings_open = false;
        settings_gui->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_rc_type_gui() {
    if (!rc_type_open) {
        hide_all_guis();
        rc_type_open = true;
        rc_type_gui->show();
    } else {
        rc_type_open = false;
        rc_type_gui->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_profile_gui() {
    if (!profile_open) {
        hide_all_guis();
        profile_open = true;
        profile_gui->show();
    } else {
        profile_open = false;
        profile_gui->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_rc_tune() {
    if (!rc_tune_open) {
        hide_all_guis();
        rc_tune_open = true;
        rc_stack->show();
    } else {
        rc_tune_open = false;
        rc_stack->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_input_tune() {
    if (!input_tune_open) {
        hide_all_guis();
        input_tune_open = true;
        input_stack->show();
    } else {
        input_tune_open = false;
        input_stack->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_rc_map() {
    if (!rc_map_open) {
        hide_all_guis();
        rc_map_open = true;
        rc_map_stack->show();
    } else {
        rc_map_open = false;
        rc_map_stack->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_trim() {
    if (!trim_open) {
        hide_all_guis();
        trim_open = true;
        trim_stack->show();
    } else {
        trim_open = false;
        trim_stack->hide();
    }
    update_panel_positions();
}
void DashboardPanel::toggle_rc_mix() {
    if (!rc_mix_open) {
        hide_all_guis();
        rc_mix_open = true;
        rc_mix_stack->show();
    } else {
        rc_mix_open = false;
        rc_mix_stack->hide();
    }
    update_panel_positions();
}
void DashboardPanel::onVehicleTypeChanged(int type) {
    main_stack->setCurrentIndex(type);
    rc_stack->setCurrentIndex(type);
    input_stack->setCurrentIndex(type);
    rc_map_stack->setCurrentIndex(type);
    trim_stack->setCurrentIndex(type);
    rc_mix_stack->setCurrentIndex(type);
}
void DashboardPanel::resizeEvent(QResizeEvent *event) {
    int w = width();
    int h = height();
    main_stack->setGeometry(0, 0, w, h);
    settings_btn->move(w - settings_btn->width() - 10, 10);
    shield->setGeometry(0, 0, w, h);
    int side_x = sidebar_open ? (w - sidebar_width) : w;
    sidebar->setGeometry(side_x, 0, sidebar_width, h);
    update_panel_positions();
}
DashboardPanel::~DashboardPanel() {}
