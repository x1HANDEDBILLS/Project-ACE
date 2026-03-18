// Location: include/vehicles/car/rc_tune_car_panel.h
#ifndef RC_TUNE_CAR_PANEL_H
#define RC_TUNE_CAR_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
class RCTuneCarPanel : public QFrame {
    Q_OBJECT
public:
    explicit RCTuneCarPanel(QWidget *parent = nullptr);
    virtual ~RCTuneCarPanel(); // Added this to match .cpp
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
