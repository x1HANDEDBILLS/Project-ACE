// Location: src/GUI/dash/main_car_dash.cpp
#include "Core/app_state.h"
#include "main_car_dash.h"
#include "theme/theme.h"
#include <QVBoxLayout>
MainCarDash::MainCarDash(QWidget *parent) : QFrame(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    header_label = new QLabel("// GROUND VEHICLE ACTIVE", this);
    telemetry_placeholder = new QLabel(this);
    header_label->setAlignment(Qt::AlignCenter);
    telemetry_placeholder->setAlignment(Qt::AlignCenter);
    telemetry_placeholder->setAttribute(Qt::WA_StaticContents);
    layout->addStretch();
    layout->addWidget(header_label);
    layout->addWidget(telemetry_placeholder);
    layout->addStretch();
    refresh_theme();
}
void MainCarDash::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    update_hud();
}
void MainCarDash::update_hud() {
    auto &t = AppState::instance().tele;
    telemetry_placeholder->setText(QString("LQ:%1% | VOLTS:%2V\nLAT:%3 LON:%4")
                                       .arg(t.lq, 3)
                                       .arg((double)t.bat, 4, 'f', 1)
                                       .arg(t.lat, 10, 'f', 6)
                                       .arg(t.lon, 10, 'f', 6));
}
void MainCarDash::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString accent = ThemeManager::instance().active().hex;
    header_label->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 22px; font-weight: bold; background: transparent;")
            .arg(silver));
    telemetry_placeholder->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-size: 24px; border: 2px "
                                                 "solid %2; padding: 20px; background: #080808;")
                                             .arg(silver, accent));
}
MainCarDash::~MainCarDash() {}
