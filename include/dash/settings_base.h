// Location: include/dash/settings_base.h
#ifndef SETTINGS_BASE_H
#define SETTINGS_BASE_H
#include "theme/theme.h"
#include <QLabel>
#include <QWidget>
class SettingsBasePage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsBasePage(QWidget *parent = nullptr) : QWidget(parent) { setAttribute(Qt::WA_StaticContents); }
    Q_INVOKABLE virtual void refresh_theme() {
        // Default behavior for placeholders
        QLabel *l = findChild<QLabel *>("placeholder_label");
        if (l) {
            QString silver = ThemeManager::instance().active().secondary.name();
            l->setStyleSheet(
                QString("color: %1; font-family: 'Consolas'; font-weight: bold; font-size: 20px;").arg(silver));
        }
    }
};
#endif
