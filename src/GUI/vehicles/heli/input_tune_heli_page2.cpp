// Location: src/GUI/vehicles/heli/input_tune_heli_page2.cpp
#include "vehicles/heli/input_tune_heli_page2.h"
#include <QLabel>
#include <QVBoxLayout>
void setup_input_tune_heli_page2_content(QWidget *page) {
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *placeholder = new QLabel("// HELI INPUT PAGE 2\n// STANDBY FOR TELEMETRY...");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 16px; border: 1px solid #222;");
    layout->addWidget(placeholder);
}
