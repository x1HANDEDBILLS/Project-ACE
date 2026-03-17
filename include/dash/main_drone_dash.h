// Location: include/dash/main_drone_dash.h
#ifndef MAIN_DRONE_DASH_H
#define MAIN_DRONE_DASH_H
#include <QFrame>
#include <QLabel>
#include <QPaintEvent>
class MainDroneDash : public QFrame {
    Q_OBJECT
public:
    explicit MainDroneDash(QWidget *parent = nullptr);
    ~MainDroneDash();
    void refresh_theme();

protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void update_hud();

private:
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};
#endif
