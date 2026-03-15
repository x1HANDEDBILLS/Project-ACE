// LOCATION: /home/user/ACE/include/vehicles/car/rc_map_car_panel.h
#ifndef RC_MAP_CAR_PANEL_H
#define RC_MAP_CAR_PANEL_H
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>

#include "widgets/button.h"
#include "widgets/frame.h"

class RCMapCarPanel : public QFrame
{
    Q_OBJECT
   public:
    explicit RCMapCarPanel(QWidget *parent = nullptr);
    ~RCMapCarPanel();
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
