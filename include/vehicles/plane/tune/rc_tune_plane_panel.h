// Location: include/vehicles/plane/rc_tune_plane_panel.h
#ifndef RC_TUNE_PLANE_PANEL_H
#define RC_TUNE_PLANE_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
class RCTunePlanePanel : public QFrame {
    Q_OBJECT
public:
    explicit RCTunePlanePanel(QWidget *parent = nullptr);
    virtual ~RCTunePlanePanel();
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
