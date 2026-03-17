// Location: include/dash/main_car_dash.h
#ifndef MAIN_CAR_DASH_H
#define MAIN_CAR_DASH_H
#include <QFrame>
#include <QLabel>
#include <QPaintEvent>
class MainCarDash : public QFrame {
    Q_OBJECT
public:
    explicit MainCarDash(QWidget *parent = nullptr);
    ~MainCarDash();
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
