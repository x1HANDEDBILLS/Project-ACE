// LOCATION: /home/user/ACE/include/dash/settings_page1.h
#ifndef SETTINGS_PAGE1_H
#define SETTINGS_PAGE1_H

#include <QVBoxLayout>
#include <QWidget>

#include "widgets/button.h"

class SettingsPage1 : public QWidget
{
    Q_OBJECT
   public:
    explicit SettingsPage1(QWidget *parent = nullptr);
    void refresh_theme();

   private slots:
    void setup_theme_popout();

   private:
    Button *theme_btn;
};
#endif
