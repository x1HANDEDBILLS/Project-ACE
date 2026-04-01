// Location: include/vehicles/drone/map/rc_map_drone_panel.h
#ifndef RC_MAP_DRONE_PANEL_H
#define RC_MAP_DRONE_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
class RCMapDronePanel : public QFrame {
    Q_OBJECT
public:
    explicit RCMapDronePanel(QWidget *parent = nullptr);
    ~RCMapDronePanel();
    void refresh_theme();
private slots:
    void next_page();
    void prev_page();
    void update_page_ui();

private:
    Frame *content_frame;
    QLabel *status_label, *header_title, *page_label;
    QStackedWidget *pages;
    Button *prev_btn, *next_btn;
    QStringList page_titles;
};
#endif
