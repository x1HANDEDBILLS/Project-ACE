// Location: src/GUI/dash/main_plane_dash.cpp
#include "Core/app_state.h"
#include "main_plane_dash.h"
#include "theme/theme.h"
#include <QVBoxLayout>
MainPlaneDash::MainPlaneDash(QWidget *parent) : QFrame(parent) {
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    // Margins: Left=90, Top=40, Right=90, Bottom=40 (Shrunk by extra 20px)
    main_layout->setContentsMargins(90, 40, 90, 40);
    main_layout->setSpacing(5);
    header_label = new QLabel("// PLANE ACTIVE", this);
    header_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(header_label);
    telemetry_placeholder = new QLabel(this);
    telemetry_placeholder->setAlignment(Qt::AlignCenter);
    telemetry_placeholder->setTextFormat(Qt::RichText);
    main_layout->addWidget(telemetry_placeholder, 1);
    refresh_theme();
}
void MainPlaneDash::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    update_hud();
}
void MainPlaneDash::update_hud() {
    auto &t = AppState::instance().tele;
    QString hex = ThemeManager::instance().active().hex;
    // Every dynamic value is colored with the active theme hex
    QString data = QString("BATT: <span style='color:%1;'>%2V</span><br>"
                           "<span style='color:#555;'>----------------------</span><br>"
                           "A1: <span style='color:%1;'>%3</span> | A2: <span style='color:%1;'>%4</span><br>"
                           "LQ: <span style='color:%1;'>%5%</span> | SNR: <span style='color:%1;'>%6</span><br>"
                           "ANT: <span style='color:%1;'>#%7</span> | PWR: <span style='color:%1;'>%8mW</span><br>"
                           "MODE: <span style='color:%1;'>%9</span><br>"
                           "<span style='color:#555;'>----------------------</span><br>"
                           "SATS: <span style='color:%1;'>%10</span> | ALT: <span style='color:%1;'>%11m</span><br>"
                           "LAT: <span style='color:%1;'>%12</span><br>"
                           "LON: <span style='color:%1;'>%13</span>")
                       .arg(hex)
                       .arg((double)t.bat, 4, 'f', 2)
                       .arg(t.rssi)
                       .arg(t.rssi2)
                       .arg(t.lq)
                       .arg(t.snr)
                       .arg(t.ant)
                       .arg(t.pwr)
                       .arg(t.mode)
                       .arg(t.sats)
                       .arg((double)t.alt, 4, 'f', 1)
                       .arg(t.lat, 9, 'f', 6)
                       .arg(t.lon, 9, 'f', 6);
    telemetry_placeholder->setText(data);
}
void MainPlaneDash::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString accent = ThemeManager::instance().active().hex;
    header_label->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 24px; font-weight: bold; letter-spacing: 4px;")
            .arg(accent));
    // Maintains massive 36px font within the tighter box
    telemetry_placeholder->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 36px; font-weight: bold; "
                "border: 3px solid %2; padding: 25px; background: rgba(0, 0, 0, 225); "
                "border-radius: 10px;")
            .arg(silver, accent));
    update();
}
MainPlaneDash::~MainPlaneDash() {}
