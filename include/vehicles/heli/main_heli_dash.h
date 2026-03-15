#ifndef MAIN_HELI_DASH_H
#define MAIN_HELI_DASH_H
#include <QFrame>
#include <QLabel>
class MainHeliDash : public QFrame
{
    Q_OBJECT
   public:
    explicit MainHeliDash(QWidget *parent = nullptr);
    ~MainHeliDash();
    void refresh_theme();

   private:
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};
#endif
