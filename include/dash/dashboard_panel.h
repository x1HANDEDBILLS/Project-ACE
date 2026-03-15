#ifndef DASHBOARD_PANEL_H
#define DASHBOARD_PANEL_H

#include <QFrame>
#include <QList>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QWidget>

#include "dash/input_read_panel.h"
#include "dash/panel_buttons.h"
#include "dash/settings_panel.h"
#include "profiles/profile_panel.h"
#include "rc_type/rc_type_panel.h"
#include "widgets/button.h"

class DashboardPanel : public QWidget
{
    Q_OBJECT
   public:
    explicit DashboardPanel(QWidget *parent = nullptr);
    ~DashboardPanel();
    void update_data();

   public slots:
    void toggle_sidebar();
    void toggle_input_gui();
    void toggle_settings_gui();
    void toggle_rc_type_gui();
    void toggle_profile_gui();
    void toggle_rc_tune();
    void toggle_input_tune();
    void toggle_rc_map();
    void onVehicleTypeChanged(int type);
    void smart_back();
    void refresh_theme();

   protected:
    void resizeEvent(QResizeEvent *event) override;

   private:
    void hide_all_guis();
    void update_panel_positions();

    Button *settings_btn;
    QFrame *sidebar;
    QFrame *shield;
    PanelButtons *button_grid;
    QPropertyAnimation *anim_side;

    InputReadPanel *input_gui;
    SettingsPanel *settings_gui;
    RCTypePanel *rc_type_gui;
    ProfilePanel *profile_gui;

    QStackedWidget *main_stack;
    QStackedWidget *rc_stack;
    QStackedWidget *input_stack;
    QStackedWidget *rc_map_stack;

    bool sidebar_open = false;
    bool input_open = false, settings_open = false, rc_type_open = false;
    bool profile_open = false, rc_tune_open = false, input_tune_open = false, rc_map_open = false;
    const int sidebar_width = 190;
};

#endif
