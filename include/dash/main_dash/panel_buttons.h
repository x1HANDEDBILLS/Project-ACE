// Location: include/dash/main_dash/panel_buttons.h
#ifndef PANEL_BUTTONS_H
#define PANEL_BUTTONS_H
#include "widgets/button.h"
#include <QList>
#include <QWidget>
class PanelButtons : public QWidget {
    Q_OBJECT
public:
    explicit PanelButtons(QWidget *parent = nullptr);
    ~PanelButtons();
    Button *back_btn;
    Button *tune_btn;
    Button *input_btn;
    Button *inner_settings_btn;
    Button *profile_btn;
    // RC Buttons
    Button *rc_type_btn;
    Button *rc_tune_btn;
    Button *rc_map_btn;
    Button *trim_btn;
    Button *rc_mix_btn;
    QList<Button *> buttons;
signals:
    void rcTypeClicked();
    void rcTuneClicked();
    void rcMapClicked();
    void trimClicked();
    void rcMixClicked();
    void inputTuneClicked();
    void profileClicked();
};
#endif
