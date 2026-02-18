#ifndef LOGIC_H
#define LOGIC_H

#include "../input/input_manage.h"
#include <cstdint>
#include <QObject>
#include <QVariantList>
#include <QList>
#include <QVector3D>
#include <QVariantMap>

class Logic : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList axes READ axes NOTIFY dataChanged)
    Q_PROPERTY(QList<bool> buttons READ buttons NOTIFY dataChanged)
    Q_PROPERTY(QVector3D gyro READ gyro NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap touch READ touch NOTIFY dataChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY dataChanged)

public:
    explicit Logic(QObject *parent = nullptr);
    ~Logic();

    bool initialize();
    void run_iteration();

    QVariantList axes() const;
    QList<bool> buttons() const;
    QVector3D gyro() const;
    QVariantMap touch() const;
    bool connected() const;

signals:
    void dataChanged();

private:
    InputManager inputManager;
    RawData m_rawData;
    uint64_t frame_count;
};

#endif