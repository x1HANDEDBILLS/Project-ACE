#ifndef MAIN_DRONE_DASH_H
#define MAIN_DRONE_DASH_H
#include <QFrame>
#include <QLabel>
class MainDroneDash : public QFrame
{
    Q_OBJECT
   public:
    explicit MainDroneDash(QWidget *parent = nullptr);
    ~MainDroneDash();
    void refresh_theme();

   private:
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};
#endif
