// LOCATION: /home/user/ACE/include/vehicles/drone/rc_tune_drone_panel.h
#ifndef RC_TUNE_DRONE_PANEL_H
#define RC_TUNE_DRONE_PANEL_H
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>

#include "widgets/button.h"
#include "widgets/frame.h"

class RCTuneDronePanel : public QFrame
{
    Q_OBJECT
   public:
    explicit RCTuneDronePanel(QWidget *parent = nullptr);
    virtual ~RCTuneDronePanel();  // Added this to match .cpp
    void refresh_theme();
   private slots:
    void next_page();
    void prev_page();

   private:
    void update_page_ui();
    Frame *content_frame;
    QLabel *status_label, *header_title, *page_label;
    QStackedWidget *pages;
    Button *prev_btn, *next_btn;
    QStringList page_titles;
};
#endif
