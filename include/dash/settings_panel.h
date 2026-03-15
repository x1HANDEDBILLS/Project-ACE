// LOCATION: /home/user/ACE/include/dash/settings_panel.h
#ifndef SETTINGS_PANEL_H
#define SETTINGS_PANEL_H

#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "widgets/button.h"
#include "widgets/frame.h"

class SettingsPanel : public QFrame
{
    Q_OBJECT
   public:
    explicit SettingsPanel(QWidget *parent = nullptr);
    void refresh_theme();

   private slots:
    void next_page();
    void prev_page();

   private:
    void update_page_ui();

    Frame *content_frame;
    QLabel *header_title;
    QLabel *status_label;
    QLabel *page_label;
    QStackedWidget *pages;

    Button *prev_btn;
    Button *next_btn;

    QStringList page_titles = {"UI THEME", "AUDIO CONFIG", "NETWORK", "SECURITY", "SYSTEM INFO"};
};
#endif
