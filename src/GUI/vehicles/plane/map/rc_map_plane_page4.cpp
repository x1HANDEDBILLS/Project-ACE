// Location: src/GUI/vehicles/plane/map/rc_map_plane_page4.cpp
#include "rc_map_plane_page4.h"
#include <QLabel>
#include <QVBoxLayout>
void setup_rc_map_plane_page4_content(QWidget *p) {
    auto *layout = new QVBoxLayout(p);
    QLabel *l = new QLabel("FAILSAFE CONFIGURATION");
    l->setAlignment(Qt::AlignCenter);
    l->setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 24px;");
    layout->addWidget(l);
}
