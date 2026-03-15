#ifndef APP_STATE_H
#define APP_STATE_H
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <cstdint>

struct SplitConfig
{
    int target_ch = -1;
    int pos_id = 22;
    int neg_id = 22;
    bool pos_center = false;
    bool pos_reverse = false;
    bool neg_center = false;
    bool neg_reverse = false;
};

struct TeleStats
{
    int rssi = 0;   // RSSI 1
    int rssi2 = 0;  // RSSI 2 (Added for diversity)
    int lq = 0;
    int snr = 0;
    int mode = 0;
    int ant = 0;  // Active Antenna (Added)
    int pwr = 0;
    float bat = 0.0f;
    double lat = 0.0, lon = 0.0;
    float alt = 0.0f;
    int sats = 0;
};

class AppState
{
   public:
    static AppState& instance();
    QString current_profile_name = "DEFAULT";
    QString theme_name = "CYAN";
    QString rc_type = "PLANE";
    float current_expo = 0.0f;
    float deadzones[8] = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f};
    bool expo_enabled[8] = {true, true, true, true, true, true, true, true};
    int16_t tuned_slots[200] = {0};
    int channel_map[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    bool inverted[16] = {false};
    int16_t channels[16] = {0};
    int16_t mapped_channels[16] = {0};
    SplitConfig split_mixers[6];
    TeleStats tele;  // New telemetry object
    QJsonObject toJson();
    void fromJson(const QJsonObject& obj);
};
#endif
