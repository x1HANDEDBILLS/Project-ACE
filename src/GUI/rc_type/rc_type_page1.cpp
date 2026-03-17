// Location: src/GUI/rc_type/rc_type_page1.cpp
#include "rc_type/rc_type_page1.h"
#include "theme.h"
#include <QVBoxLayout>
RCTypePage1::RCTypePage1(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(30, 30, 30, 30);
    selector = new RCTypeSelector(this);
    main_layout->addWidget(selector, 0, Qt::AlignCenter);
    main_layout->addStretch();
}
void RCTypePage1::refresh_theme() {
    // Logic for sub-widget theme updates goes here if needed
}