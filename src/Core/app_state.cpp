#include "Core/app_state.h"

AppState& AppState::instance()
{
    static AppState inst;
    return inst;
}

QJsonObject AppState::toJson()
{
    QJsonObject obj;
    obj["profile_name"] = current_profile_name;
    obj["theme_name"] = theme_name;
    obj["rc_type"] = rc_type;
    obj["expo"] = (double)current_expo;

    QJsonArray dz;
    for (int i = 0; i < 8; i++) dz.append((double)deadzones[i]);
    obj["deadzones"] = dz;

    QJsonArray ex;
    for (int i = 0; i < 8; i++) ex.append(expo_enabled[i]);
    obj["expo_enabled"] = ex;

    QJsonArray map;
    for (int i = 0; i < 16; ++i) map.append(channel_map[i]);
    obj["channel_map"] = map;

    QJsonArray inv;
    for (int i = 0; i < 16; ++i) inv.append(inverted[i]);
    obj["inverted"] = inv;

    QJsonArray mixers;
    for (int i = 0; i < 6; i++) {
        QJsonObject m;
        m["target"] = split_mixers[i].target_ch;
        m["p_id"] = split_mixers[i].pos_id;
        m["n_id"] = split_mixers[i].neg_id;
        m["p_c"] = split_mixers[i].pos_center;
        m["p_r"] = split_mixers[i].pos_reverse;
        m["n_c"] = split_mixers[i].neg_center;
        m["n_r"] = split_mixers[i].neg_reverse;
        mixers.append(m);
    }
    obj["mixers"] = mixers;
    return obj;
}

void AppState::fromJson(const QJsonObject& obj)
{
    if (obj.contains("profile_name"))
        current_profile_name = obj["profile_name"].toString("DEFAULT");
    if (obj.contains("theme_name")) theme_name = obj["theme_name"].toString("CYAN");
    if (obj.contains("rc_type")) rc_type = obj["rc_type"].toString("PLANE");

    if (obj.contains("channel_map")) {
        QJsonArray map = obj["channel_map"].toArray();
        for (int i = 0; i < 16 && i < map.size(); ++i) channel_map[i] = map[i].toInt();
    }

    if (obj.contains("inverted")) {
        QJsonArray inv = obj["inverted"].toArray();
        for (int i = 0; i < 16 && i < inv.size(); ++i) inverted[i] = inv[i].toBool(false);
    }

    if (obj.contains("mixers")) {
        QJsonArray mixers = obj["mixers"].toArray();
        for (int i = 0; i < 6 && i < mixers.size(); i++) {
            QJsonObject m = mixers[i].toObject();
            split_mixers[i].target_ch = m["target"].toInt(-1);
            split_mixers[i].pos_id = m["p_id"].toInt(22);
            split_mixers[i].neg_id = m["n_id"].toInt(22);
            split_mixers[i].pos_center = m["p_c"].toBool(false);
            split_mixers[i].pos_reverse = m["p_r"].toBool(false);
            split_mixers[i].neg_center = m["n_c"].toBool(false);
            split_mixers[i].neg_reverse = m["n_r"].toBool(false);
        }
    }
}
