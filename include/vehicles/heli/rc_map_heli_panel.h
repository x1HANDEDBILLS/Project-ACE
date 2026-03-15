// LOCATION: /home/user/ACE/include/vehicles/heli/rc_map_heli_panel.h
#ifndef RC_MAP_HELI_PANEL_H
#define RC_MAP_HELI_PANEL_H
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>

#include "widgets/button.h"
#include "widgets/frame.h"

class RCMapHeliPanel : public QFrame
{
    Q_OBJECT
   public:
    explicit RCMapHeliPanel(QWidget *parent = nullptr);
    ~RCMapHeliPanel();
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
