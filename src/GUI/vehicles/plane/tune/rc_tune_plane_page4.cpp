// Location: src/GUI/vehicles/plane/tune/rc_tune_plane_page4.cpp
#include "rc_tune_plane_page4.h"
#include <QLabel>
#include <QVBoxLayout>
void setup_rc_tune_plane_page4_content(QWidget *page) {
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *placeholder = new QLabel("// PLANE RC PAGE 4\n// STANDBY FOR TELEMETRY...");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 16px; border: 1px solid #222;");
    layout->addWidget(placeholder);
}
