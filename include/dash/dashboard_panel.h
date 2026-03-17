// Location: include/dash/dashboard_panel.h
#ifndef DASHBOARD_PANEL_H
#define DASHBOARD_PANEL_H
#include "dash/input_read_panel.h"
#include "dash/panel_buttons.h"
#include "dash/settings_panel.h"
#include "profiles/profile_panel.h"
#include "rc_type/rc_type_panel.h"
#include "widgets/button.h"
#include <QFrame>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QWidget>
class DashboardPanel : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPanel(QWidget *parent = nullptr);
    ~DashboardPanel();
    void update_data();
    void refresh_theme();

protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void toggle_sidebar();
    void toggle_input_gui();
    void toggle_settings_gui();
    void toggle_rc_type_gui();
    void toggle_profile_gui();
    void toggle_rc_tune();
    void toggle_input_tune();
    void toggle_rc_map();
    void toggle_trim();
    void toggle_rc_mix();
    void smart_back();
    void onVehicleTypeChanged(int type);

private:
    void update_panel_positions();
    void hide_all_guis();
    QStackedWidget *main_stack;
    QStackedWidget *rc_stack;
    QStackedWidget *input_stack;
    QStackedWidget *rc_map_stack;
    QStackedWidget *trim_stack;
    QStackedWidget *rc_mix_stack;
    QFrame *sidebar;
    QFrame *shield;
    PanelButtons *button_grid;
    Button *settings_btn;
    InputReadPanel *input_gui;
    SettingsPanel *settings_gui;
    RCTypePanel *rc_type_gui;
    ProfilePanel *profile_gui;
    QPropertyAnimation *anim_side;
    bool sidebar_open = false;
    bool input_open = false;
    bool settings_open = false;
    bool rc_type_open = false;
    bool profile_open = false;
    bool rc_tune_open = false;
    bool input_tune_open = false;
    bool rc_map_open = false;
    bool trim_open = false;
    bool rc_mix_open = false;
    const int sidebar_width = 200;
};
#endif
