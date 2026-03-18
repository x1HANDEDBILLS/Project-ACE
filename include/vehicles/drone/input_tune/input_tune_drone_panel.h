// Location: include/vehicles/drone/input_tune_drone_panel.h
#ifndef INPUT_TUNE_DRONE_PANEL_H
#define INPUT_TUNE_DRONE_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
class InputTuneDronePanel : public QFrame {
    Q_OBJECT
public:
    explicit InputTuneDronePanel(QWidget *parent = nullptr);
    ~InputTuneDronePanel();
    void refresh_theme();
private slots:
    void next_page();
    void prev_page();

private:
    void update_page_ui();
    Frame *content_frame;
    QLabel *status_label;
    QLabel *header_title;
    QLabel *page_label;
    QStackedWidget *pages;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;
};
#endif
