#include "vehicles/plane/main_plane_dash.h"

#include <QTimer>
#include <QVBoxLayout>

#include "Core/app_state.h"
#include "theme/theme.h"

MainPlaneDash::MainPlaneDash(QWidget *parent) : QFrame(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    header_label = new QLabel("// PLANE ACTIVE", this);
    telemetry_placeholder = new QLabel(this);

    header_label->setAlignment(Qt::AlignCenter);
    telemetry_placeholder->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(header_label);
    layout->addWidget(telemetry_placeholder);
    layout->addStretch();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainPlaneDash::update_hud);
    timer->start(100);
    refresh_theme();
}

void MainPlaneDash::update_hud()
{
    auto &t = AppState::instance().tele;

    // Formatting line 1 to match your printf: A1, A2, SNR, LQ, Mode, Ant, Bat
    QString line1 = QString("A1:%1 A2:%2 | SNR:%3 | LQ:%4% | Mode:%5 Ant:%6 | Bat:%7V")
                        .arg(t.rssi, 3)
                        .arg(t.rssi2, 3)
                        .arg(t.snr, 2)
                        .arg(t.lq, 3)
                        .arg(t.mode)
                        .arg(t.ant)
                        .arg((double)t.bat, 4, 'f', 1);

    QString line2 = QString("\nGPS:%1 | Alt:%2m | L:%3, %4")
                        .arg(t.sats, 2)
                        .arg((double)t.alt, 4, 'f', 0)
                        .arg(t.lat, 10, 'f', 6)
                        .arg(t.lon, 10, 'f', 6);

    telemetry_placeholder->setText(line1 + line2);
}

void MainPlaneDash::refresh_theme()
{
    QString silver = ThemeManager::instance().active().secondary.name();
    QString accent = ThemeManager::instance().active().hex;
    header_label->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 20px; font-weight: bold;")
            .arg(silver));
    telemetry_placeholder->setStyleSheet(
        QString("color: %1; font-family: 'Consolas'; font-size: 17px; "
                "border: 2px solid %2; padding: 15px; background: rgba(0, 0, 0, 160);")
            .arg(silver, accent));
}

MainPlaneDash::~MainPlaneDash() {}
