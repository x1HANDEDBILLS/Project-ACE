#ifndef RC_MAP_PLANE_PAGE3_H
#define RC_MAP_PLANE_PAGE3_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Core/app_state.h"

class RCMapPlanePage3 : public QWidget
{
    Q_OBJECT
   public:
    explicit RCMapPlanePage3(QWidget* parent = nullptr);

   public slots:
    void refresh_ui();

   private:
    QPushButton* rev_buttons[16];
    void update_button_style(int i);
};

#endif
