// Location: src/Core/profile_manager.cpp
#include "Core/app_state.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
void ProfileManager::save() {
    // Redirect generic save to currently active category/slot
    save_to_category(AppState::instance().rc_type, 0, AppState::instance().current_profile_name);
}
void ProfileManager::save_to_category(const QString &cat, int slot_id, const QString &name) {
    QDir().mkpath("profiles/" + cat);
    QString path = QString("profiles/%1/slot_%2.json").arg(cat).arg(slot_id);
    AppState::instance().current_profile_name = name;
    AppState::instance().rc_type = cat;
    AppState::instance().theme_name = ThemeManager::instance().active().name;
    QJsonObject root = AppState::instance().toJson();
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        QFile last("profiles/last_session.ptr");
        if (last.open(QIODevice::WriteOnly)) last.write(path.toUtf8());
    }
}
void ProfileManager::load_from_category(const QString &cat, int slot_id) {
    QString path = QString("profiles/%1/slot_%2.json").arg(cat).arg(slot_id);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        AppState::instance().fromJson(obj);
        ThemeManager::instance().set_theme(AppState::instance().theme_name);
        emit profileLoaded(obj);
    }
}
void ProfileManager::boot_load() {
    QFile last("profiles/last_session.ptr");
    if (last.open(QIODevice::ReadOnly)) {
        QString last_path = QString::fromUtf8(last.readAll());
        QFile file(last_path);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
            AppState::instance().fromJson(obj);
            ThemeManager::instance().set_theme(AppState::instance().theme_name);
            emit profileLoaded(obj);
        }
    }
}
QString ProfileManager::get_slot_name(const QString &cat, int slot_id) {
    QString path = QString("profiles/%1/slot_%2.json").arg(cat).arg(slot_id);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        return obj["profile_name"].toString(QString("Slot %1").arg(slot_id));
    }
    return QString("Empty Slot %1").arg(slot_id);
}
