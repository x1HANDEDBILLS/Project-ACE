#ifndef INPUT_TUNE_PLANE_PANEL_H
#define INPUT_TUNE_PLANE_PANEL_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
#include "widgets/frame.h"
#include "widgets/button.h"
#include "widgets/keypad.h"

class InputTunePlanePanel : public QWidget {
    Q_OBJECT
public:
    explicit InputTunePlanePanel(QWidget *parent = nullptr);
    ~InputTunePlanePanel();

    void next_page();
    void prev_page();
    void update_page_ui();
    void refresh_theme();
    
    // The "Bridge" for the new Overlay Keypad
    void open_keypad(float *target, class TuneRow *row);

private:
    Frame *content_frame;
    QStackedWidget *stack;
    QLabel *status_label;
    QLabel *header_title;
    QLabel *page_label;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;

    // The Persistent Overlay Keypad
    Keypad *overlay_keypad;
    QFrame *keypad_shield;
    float *active_target = nullptr;
    class TuneRow *active_row = nullptr;
};

#endif
