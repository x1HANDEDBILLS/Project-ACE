// LOCATION: /home/user/ACE/src/GUI/vehicles/drone/rc_tune_drone_page3.cpp
#include "vehicles/drone/rc_tune_drone_page3.h"

#include <QLabel>
#include <QVBoxLayout>

void setup_rc_tune_drone_page3_content(QWidget *page)
{
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *placeholder = new QLabel("// DRONE RC PAGE 3\n// STANDBY FOR TELEMETRY...");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet(
        "color: #444; font-family: 'Consolas'; font-size: 16px; border: 1px solid #222;");
    layout->addWidget(placeholder);
}
