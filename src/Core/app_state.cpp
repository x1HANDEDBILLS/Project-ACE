// Location: src/Core/app_state.cpp
#include "Core/app_state.h"
#include <QJsonArray>
AppState &AppState::instance() {
    static AppState inst;
    return inst;
}
QJsonObject AppState::toJson() {
    QJsonObject obj;
    obj["profile_name"] = current_profile_name;
    obj["theme_name"] = theme_name;
    obj["rc_type"] = rc_type;
    obj["expo"] = (double)current_expo;
    QJsonArray map, inv, thm, p_trim, s_trim, dz;
    for (int i = 0; i < 16; ++i) {
        map.append(channel_map[i]);
        inv.append(inverted[i]);
        thm.append(throttle_mode[i]);
        p_trim.append(physical_trims[i]);
        s_trim.append(sub_trims[i]);
        if (i < 8) dz.append((double)deadzones[i]);
    }
    obj["channel_map"] = map;
    obj["inverted"] = inv;
    obj["throttle_mode"] = thm;
    obj["physical_trims"] = p_trim;
    obj["sub_trims"] = s_trim;
    obj["deadzones"] = dz;
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
void AppState::fromJson(const QJsonObject &obj) {
    if (obj.contains("profile_name")) current_profile_name = obj["profile_name"].toString();
    if (obj.contains("theme_name")) theme_name = obj["theme_name"].toString();
    if (obj.contains("rc_type")) rc_type = obj["rc_type"].toString();
    if (obj.contains("expo")) current_expo = (float)obj["expo"].toDouble();
    auto loadIntArray = [&](const QString &key, int *dest, int max) {
        if (obj.contains(key)) {
            QJsonArray arr = obj[key].toArray();
            for (int i = 0; i < max && i < arr.size(); ++i) dest[i] = arr[i].toInt();
        }
    };
    auto loadInt16Array = [&](const QString &key, int16_t *dest, int max) {
        if (obj.contains(key)) {
            QJsonArray arr = obj[key].toArray();
            for (int i = 0; i < max && i < arr.size(); ++i) dest[i] = (int16_t)arr[i].toInt();
        }
    };
    auto loadBoolArray = [&](const QString &key, bool *dest, int max) {
        if (obj.contains(key)) {
            QJsonArray arr = obj[key].toArray();
            for (int i = 0; i < max && i < arr.size(); ++i) dest[i] = arr[i].toBool();
        }
    };
    loadIntArray("channel_map", channel_map, 16);
    loadBoolArray("inverted", inverted, 16);
    loadBoolArray("throttle_mode", throttle_mode, 16);
    loadInt16Array("physical_trims", physical_trims, 16);
    loadInt16Array("sub_trims", sub_trims, 16);
    if (obj.contains("deadzones")) {
        QJsonArray arr = obj["deadzones"].toArray();
        for (int i = 0; i < 8 && i < arr.size(); ++i) deadzones[i] = (float)arr[i].toDouble();
    }
    if (obj.contains("mixers")) {
        QJsonArray arr = obj["mixers"].toArray();
        for (int i = 0; i < 6 && i < arr.size(); i++) {
            QJsonObject m = arr[i].toObject();
            split_mixers[i].target_ch = m["target"].toInt();
            split_mixers[i].pos_id = m["p_id"].toInt();
            split_mixers[i].neg_id = m["n_id"].toInt();
            split_mixers[i].pos_center = m["p_c"].toBool();
            split_mixers[i].pos_reverse = m["p_r"].toBool();
            split_mixers[i].neg_center = m["n_c"].toBool();
            split_mixers[i].neg_reverse = m["n_r"].toBool();
        }
    }
}
