// Location: include/vehicles/plane/rc_trim_plane_panel.h
#ifndef RC_TRIM_PLANE_PANEL_H
#define RC_TRIM_PLANE_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
#include <QWidget>
class RCTrimPlanePanel : public QWidget {
    Q_OBJECT
public:
    explicit RCTrimPlanePanel(QWidget *parent = nullptr);
    ~RCTrimPlanePanel();
public slots:
    void update_page_ui();
    void refresh_theme();
    void next_page();
    void prev_page();

private:
    Frame *content_frame;
    QLabel *status_label;
    QLabel *header_title;
    QLabel *page_label;
    QStackedWidget *pages;
    QStringList page_titles;
    Button *prev_btn;
    Button *next_btn;
};
#endif
