// Location: include/vehicles/plane/rc_mix_plane_panel.h
#pragma once
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
class RCMixPlanePanel : public QFrame {
    Q_OBJECT
public:
    explicit RCMixPlanePanel(QWidget *parent = nullptr);
    virtual ~RCMixPlanePanel();
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
