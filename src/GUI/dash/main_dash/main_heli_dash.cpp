// Location: src/GUI/dash/main_heli_dash.cpp
#include "Core/app_state.h"
#include "main_heli_dash.h"
#include "theme/theme.h"
#include <QVBoxLayout>
MainHeliDash::MainHeliDash(QWidget *parent) : QFrame(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    header_label = new QLabel("// HELICOPTER ACTIVE", this);
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
void MainHeliDash::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    update_hud();
}
void MainHeliDash::update_hud() {
    auto &t = AppState::instance().tele;
    // Standardizing Heli HUD
    telemetry_placeholder->setText(QString("LQ:%1% | BAT:%2V\nALT:%3m | SATS:%4")
                                       .arg(t.lq, 3)
                                       .arg((double)t.bat, 4, 'f', 1)
                                       .arg((double)t.alt, 4, 'f', 0)
                                       .arg(t.sats, 2));
}
void MainHeliDash::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString accent = ThemeManager::instance().active().hex;
    header_label->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 22px; font-weight: bold; background: transparent;")
            .arg(silver));
    telemetry_placeholder->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-size: 24px; border: 2px "
                                                 "solid %2; padding: 20px; background: #080808;")
                                             .arg(silver, accent));
}
MainHeliDash::~MainHeliDash() {}
