#ifndef MAIN_PLANE_DASH_H
#define MAIN_PLANE_DASH_H

#include <QFrame>
#include <QLabel>

class MainPlaneDash : public QFrame
{
    Q_OBJECT

   public:
    explicit MainPlaneDash(QWidget *parent = nullptr);
    ~MainPlaneDash();
    void refresh_theme();

   private slots:
    void update_hud();  // Added this declaration

   private:
    QLabel *header_label;
    QLabel *telemetry_placeholder;
};

#endif
