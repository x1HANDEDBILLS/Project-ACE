#include "Core/app_state.h"
#include <QJsonArray>

AppState &AppState::instance() {
    static AppState inst;
    return inst;
}

QJsonObject AppState::toJson() {
    QJsonObject obj;
    obj["profile"] = current_profile_name;
    obj["rc_type"] = rc_type;
    obj["theme"] = theme_name;

    // 200-Slot Conditioning
    QJsonArray dz; for (int i = 0; i < 200; i++) dz.append(deadzones[i]);
    obj["deadzones"] = dz;
    QJsonArray adz; for (int i = 0; i < 200; i++) adz.append(axial_deadzones[i]);
    obj["axial_dz"] = adz;
    QJsonArray exv; for (int i = 0; i < 200; i++) exv.append(expo_values[i]);
    obj["expo_vals"] = exv;
    QJsonArray exen; for (int i = 0; i < 200; i++) exen.append(expo_enabled[i]);
    obj["expo_en"] = exen;

    // Page 3: Channel Config & Logic
    QJsonArray cmap; for (int i = 0; i < 16; i++) cmap.append(channel_map[i]);
    obj["cmap"] = cmap;
    QJsonArray inv; for (int i = 0; i < 16; i++) inv.append(inverted[i]);
    obj["inverted"] = inv;
    QJsonArray thr; for (int i = 0; i < 16; i++) thr.append(throttle_mode[i]);
    obj["throttle_mode"] = thr;

    // Page 4: Trims
    QJsonArray pt; for (int i = 0; i < 16; i++) pt.append(physical_trims[i]);
    obj["phys_trims"] = pt;
    QJsonArray st; for (int i = 0; i < 16; i++) st.append(sub_trims[i]);
    obj["sub_trims"] = st;

    // Page 2: Mixers
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
    if (obj.contains("profile")) current_profile_name = obj["profile"].toString();
    if (obj.contains("rc_type")) rc_type = obj["rc_type"].toString();
    if (obj.contains("theme")) theme_name = obj["theme"].toString();
    
    // Conditioning Load
    if (obj.contains("deadzones")) {
        QJsonArray arr = obj["deadzones"].toArray();
        for (int i = 0; i < 200 && i < arr.size(); i++) deadzones[i] = (float)arr[i].toDouble();
    }
    if (obj.contains("axial_dz")) {
        QJsonArray arr = obj["axial_dz"].toArray();
        for (int i = 0; i < 200 && i < arr.size(); i++) axial_deadzones[i] = (float)arr[i].toDouble();
    }
    if (obj.contains("expo_vals")) {
        QJsonArray arr = obj["expo_vals"].toArray();
        for (int i = 0; i < 200 && i < arr.size(); i++) expo_values[i] = (float)arr[i].toDouble();
    }
    if (obj.contains("expo_en")) {
        QJsonArray arr = obj["expo_en"].toArray();
        for (int i = 0; i < 200 && i < arr.size(); i++) expo_enabled[i] = arr[i].toBool();
    }

    // Page 3 Load
    if (obj.contains("cmap")) {
        QJsonArray arr = obj["cmap"].toArray();
        for (int i = 0; i < 16 && i < arr.size(); i++) channel_map[i] = arr[i].toInt();
    }
    if (obj.contains("inverted")) {
        QJsonArray arr = obj["inverted"].toArray();
        for (int i = 0; i < 16 && i < arr.size(); i++) inverted[i] = arr[i].toBool();
    }
    if (obj.contains("throttle_mode")) {
        QJsonArray arr = obj["throttle_mode"].toArray();
        for (int i = 0; i < 16 && i < arr.size(); i++) throttle_mode[i] = arr[i].toBool();
    }

    // Page 4 Load
    if (obj.contains("phys_trims")) {
        QJsonArray arr = obj["phys_trims"].toArray();
        for (int i = 0; i < 16 && i < arr.size(); i++) physical_trims[i] = (int16_t)arr[i].toInt();
    }
    if (obj.contains("sub_trims")) {
        QJsonArray arr = obj["sub_trims"].toArray();
        for (int i = 0; i < 16 && i < arr.size(); i++) sub_trims[i] = (int16_t)arr[i].toInt();
    }

    // Mixers Load
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
