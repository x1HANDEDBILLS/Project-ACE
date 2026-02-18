#include "logic.h"
#include <cstdio>
#include <cstring>   // for memcmp

Logic::Logic(QObject *parent) : QObject(parent), frame_count(0) {
    // Keep startup clear screen if you want, but no cursor hide/show spam
    // printf("\033[2J\033[H\033[?25l");  // Comment this out if you want zero console output
}

Logic::~Logic() { 
    // printf("\033[?25h\n");  // Comment this out
}

bool Logic::initialize() {
    return inputManager.initialize();
}

void Logic::run_iteration() {
    inputManager.update();
    RawData newData = inputManager.getCombined();

    bool hasChanged = (std::memcmp(&newData, &m_rawData, sizeof(RawData)) != 0);
    m_rawData = newData;

    if (hasChanged) {
        emit dataChanged();
    }

    // ALL console output removed — no printf calls here anymore
    // The box display and repeated controller messages are gone

    frame_count++;
}

QVariantList Logic::axes() const {
    QVariantList list;
    for (int i = 0; i < 4; ++i) {
        list.append(m_rawData.axes[i]);
    }
    return list;
}

QList<bool> Logic::buttons() const {
    QList<bool> list;
    for (int i = 0; i < 16; ++i) {
        list.append(m_rawData.buttons[i]);
    }
    return list;
}

QVector3D Logic::gyro() const {
    return QVector3D(m_rawData.gyro[0], m_rawData.gyro[1], m_rawData.gyro[2]);
}

QVariantMap Logic::touch() const {
    QVariantMap map;
    map["x"] = m_rawData.touch[0].x;
    map["y"] = m_rawData.touch[0].y;
    map["down"] = m_rawData.touch[0].down;
    return map;
}

bool Logic::connected() const {
    return m_rawData.connected;
}