// Location: include/dash/settings_page1.h
#ifndef SETTINGS_PAGE1_H
#define SETTINGS_PAGE1_H
#include "widgets/button.h"
#include <QWidget>
class SettingsPage1 : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage1(QWidget *parent = nullptr);
    // Q_INVOKABLE allows QMetaObject::invokeMethod to find this
    Q_INVOKABLE void refresh_theme();
private slots:
    void setup_theme_popout();

private:
    Button *theme_btn;
};
#endif
