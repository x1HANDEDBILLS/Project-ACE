// Location: include/rc_type/rc_type_page1.h
#ifndef RC_TYPE_PAGE1_H
#define RC_TYPE_PAGE1_H
#include "rc_type/rc_type_selector.h"
#include <QWidget>
class RCTypePage1 : public QWidget {
    Q_OBJECT
public:
    explicit RCTypePage1(QWidget *parent = nullptr);
    void refresh_theme();

private:
    RCTypeSelector *selector;
};
#endif