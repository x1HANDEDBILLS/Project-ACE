#ifndef MAIN_CAR_DASH_H
#define MAIN_CAR_DASH_H
#include <QFrame>
#include <QLabel>
class MainCarDash : public QFrame
{
    Q_OBJECT
   public:
    explicit MainCarDash(QWidget *parent = nullptr);
    ~MainCarDash();
    void refresh_theme();

   private:
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};
#endif
