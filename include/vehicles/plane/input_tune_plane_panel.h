// LOCATION: /home/user/ACE/include/vehicles/plane/input_tune_plane_panel.h
#pragma once
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>

#include "widgets/button.h"
#include "widgets/frame.h"

class InputTunePlanePanel : public QFrame
{
    Q_OBJECT
   public:
    explicit InputTunePlanePanel(QWidget *parent = nullptr);
    virtual ~InputTunePlanePanel();
    void refresh_theme();
   private slots:
    void next_page();
    void prev_page();

   private:
    void update_page_ui();
    Frame *content_frame;
    QStackedWidget *pages;
    QLabel *status_label;
    QLabel *header_title;
    QLabel *page_label;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;
};
