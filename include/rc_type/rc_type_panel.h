// Location: include/rc_type/rc_type_panel.h
#ifndef RC_TYPE_PANEL_H
#define RC_TYPE_PANEL_H
#include "rc_type/rc_type_selector.h"
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
class RCTypePanel : public QFrame {
    Q_OBJECT
public:
    explicit RCTypePanel(QWidget *parent = nullptr);
    void refresh_theme();
    RCTypeSelector *getSelector() { return selector; }
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
    RCTypeSelector *selector;
    QStringList page_titles = {"AIRCRAFT SELECT", "WING CONFIG", "TAIL TYPE", "MOTOR SETUP", "OUTPUT MAPPING"};
};
#endif