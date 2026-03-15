// LOCATION: /home/user/ACE/include/profiles/profile_manager.h
#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class ProfileManager : public QObject
{
    Q_OBJECT
   public:
    void save();
    static ProfileManager &instance()
    {
        static ProfileManager inst;
        return inst;
    }

    void save_to_category(const QString &cat, int slot_id, const QString &name);
    void load_from_category(const QString &cat, int slot_id);
    QString get_slot_name(const QString &cat, int slot_id);

    void boot_load();

   signals:
    void profileLoaded(const QJsonObject &data);

   private:
    explicit ProfileManager(QObject *parent = nullptr) : QObject(parent) {}
};

#endif
