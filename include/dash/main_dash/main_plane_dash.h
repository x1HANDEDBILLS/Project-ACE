// Location: include/dash/main_dash/main_plane_dash.h
#ifndef MAIN_PLANE_DASH_H
#define MAIN_PLANE_DASH_H
#include <QFrame>
#include <QLabel>
class MainPlaneDash : public QFrame {
    Q_OBJECT
public:
    explicit MainPlaneDash(QWidget *parent = nullptr);
    virtual ~MainPlaneDash();
    // Called by the Global UI Pulse
    Q_INVOKABLE void refresh_theme();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void update_hud();
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};
#endif
