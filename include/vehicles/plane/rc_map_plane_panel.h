#ifndef RC_MAP_PLANE_PANEL_H
#define RC_MAP_PLANE_PANEL_H

#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>

#include "widgets/button.h"
#include "widgets/frame.h"

class RCMapPlanePanel : public QFrame
{
    Q_OBJECT
   public:
    explicit RCMapPlanePanel(QWidget *parent = nullptr);
    ~RCMapPlanePanel();
    void refresh_theme();

   private slots:
    void next_page();
    void prev_page();

   private:
    void update_page_ui();
    Frame *content_frame;
    QLabel *header_title;
    QLabel *status_label;
    QLabel *page_label;
    QStackedWidget *pages;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;
};
#endif
