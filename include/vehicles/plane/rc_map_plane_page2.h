#ifndef RC_MAP_PLANE_PAGE2_H
#define RC_MAP_PLANE_PAGE2_H

#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Core/app_state.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"

class RCMapPlanePage2 : public QWidget
{
    Q_OBJECT
   public:
    explicit RCMapPlanePage2(QWidget* parent = nullptr);

   public slots:
    void refresh_ui();

   private:
    struct MixerUI
    {
        QPushButton* target_btn;
        QPushButton* pos_id_btn;
        QPushButton* neg_id_btn;
        QCheckBox* pos_ctr;
        QCheckBox* pos_rev;
        QCheckBox* neg_ctr;
        QCheckBox* neg_rev;
    };
    MixerUI mixers[6];
    void update_target_style(int i);
};

#endif
