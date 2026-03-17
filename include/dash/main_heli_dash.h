// Location: include/dash/main_heli_dash.h
#ifndef MAIN_HELI_DASH_H
#define MAIN_HELI_DASH_H
#include <QFrame>
#include <QLabel>
#include <QPaintEvent>
class MainHeliDash : public QFrame {
    Q_OBJECT
public:
    explicit MainHeliDash(QWidget *parent = nullptr);
    ~MainHeliDash();
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
