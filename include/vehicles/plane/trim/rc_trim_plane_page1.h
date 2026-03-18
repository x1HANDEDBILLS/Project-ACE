// Location: include/vehicles/plane/rc_trim_plane_page1.h
#ifndef RC_TRIM_PLANE_PAGE1_H
#define RC_TRIM_PLANE_PAGE1_H
#include <QWidget>
class RCTrimPlanePage1 : public QWidget {
public:
    explicit RCTrimPlanePage1(QWidget *parent = nullptr);
    void refresh_ui();
};
#endif
