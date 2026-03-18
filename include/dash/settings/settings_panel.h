// Location: include/dash/settings_panel.h
#ifndef SETTINGS_PANEL_H
#define SETTINGS_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
class SettingsPanel : public QFrame {
    Q_OBJECT
public:
    explicit SettingsPanel(QWidget *parent = nullptr);
    virtual ~SettingsPanel(); // Declared to match .cpp definition
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
    QStringList page_titles;
};
#endif
