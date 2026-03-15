// LOCATION: /home/user/ACE/include/profiles/profile_panel.h
#ifndef PROFILE_PANEL_H
#define PROFILE_PANEL_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

#include "widgets/button.h"
#include "widgets/frame.h"

class ProfilePanel : public QFrame
{
    Q_OBJECT
   public:
    explicit ProfilePanel(QWidget *parent = nullptr);
    void refresh_theme();

   private slots:
    void switch_category(const QString &cat);

   private:
    void show_slot_options(int slot_id, Button *btn);
    void open_keyboard(int slot_id, Button *btn);
    void trigger_loading_sequence(const QString &cat, int slot_id);

    Frame *content_frame;
    QLabel *status_label, *header_title;
    QGridLayout *profile_grid;
    QWidget *grid_container;
    QString current_category;
};

#endif
