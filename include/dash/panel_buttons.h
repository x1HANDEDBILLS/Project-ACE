// LOCATION: /home/user/ACE/include/dash/panel_buttons.h
#ifndef PANEL_BUTTONS_H
#define PANEL_BUTTONS_H

#include <QList>
#include <QWidget>

#include "widgets/button.h"

class PanelButtons : public QWidget
{
    Q_OBJECT
   public:
    explicit PanelButtons(QWidget *parent = nullptr);
    ~PanelButtons();

    Button *back_btn;
    Button *tune_btn;            // Input Tune
    Button *input_btn;           // Input Monitor
    Button *inner_settings_btn;  // Settings
    Button *profile_btn;

    // RC Buttons
    Button *rc_type_btn;  // Index 0
    Button *rc_tune_btn;  // Index 2
    Button *rc_map_btn;   // Index 4 (Added)

    QList<Button *> buttons;

   signals:
    void rcTypeClicked();
    void rcTuneClicked();
    void rcMapClicked();  // Added
    void inputTuneClicked();
    void profileClicked();
};
#endif