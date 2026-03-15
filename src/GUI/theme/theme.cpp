// LOCATION: /home/user/ACE/src/GUI/theme/theme.cpp
#include "theme.h"

ThemeManager& ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager()
{
    init();
}

void ThemeManager::init()
{
    QColor silver(192, 192, 192);
    themes = {{"CYAN", "#00ffff", silver, silver, QColor(15, 35, 40), QColor(5, 10, 15)},
              {"RED", "#ff4444", silver, silver, QColor(35, 10, 10), QColor(15, 5, 5)},
              {"GREEN", "#00ff00", silver, silver, QColor(10, 35, 10), QColor(5, 15, 5)},
              {"AMBER", "#ffb000", silver, silver, QColor(35, 25, 5), QColor(15, 10, 2)}};
    active_theme = themes[0];
}

void ThemeManager::next()
{
    static int idx = 0;
    idx = (idx + 1) % (int)themes.size();
    active_theme = themes[idx];
    emit themeChanged();
}

void ThemeManager::set_theme(const QString& name)
{
    for (const auto& t : themes) {
        if (t.name == name) {
            active_theme = t;
            emit themeChanged();
            break;
        }
    }
}
