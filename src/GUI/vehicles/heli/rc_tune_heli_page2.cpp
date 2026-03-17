// Location: src/GUI/vehicles/heli/rc_tune_heli_page2.cpp
#include "vehicles/heli/rc_tune_heli_page2.h"
#include <QLabel>
#include <QVBoxLayout>
void setup_rc_tune_heli_page2_content(QWidget *page) {
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *placeholder = new QLabel("// HELI RC PAGE 2\n// STANDBY FOR TELEMETRY...");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 16px; border: 1px solid #222;");
    layout->addWidget(placeholder);
}
