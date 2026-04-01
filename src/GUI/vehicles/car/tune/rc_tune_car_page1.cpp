// Location: src/GUI/vehicles/car/tune/rc_tune_car_page1.cpp
#include "rc_tune_car_page1.h"
#include <QLabel>
#include <QVBoxLayout>
void setup_rc_tune_car_page1_content(QWidget *page) {
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *placeholder = new QLabel("// CAR RC PAGE 1\n// STANDBY FOR TELEMETRY...");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 16px; border: 1px solid #222;");
    layout->addWidget(placeholder);
}
