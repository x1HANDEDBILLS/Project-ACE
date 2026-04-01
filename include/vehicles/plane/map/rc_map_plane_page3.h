// Location: include/vehicles/plane/map/rc_map_plane_page3.h
#ifndef RC_MAP_PLANE_PAGE3_H
#define RC_MAP_PLANE_PAGE3_H
#include "Core/app_state.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
class RCMapPlanePage3 : public QWidget {
    Q_OBJECT
public:
    explicit RCMapPlanePage3(QWidget *parent = nullptr);
public slots:
    void refresh_ui();

private:
    QPushButton *rev_buttons[16];
    QPushButton *thr_buttons[16]; // Added to store the throttle mode buttons
};
#endif
