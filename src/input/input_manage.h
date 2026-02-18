#ifndef INPUT_MANAGE_H
#define INPUT_MANAGE_H

#include <QObject>

class InputManage : public QObject {
    Q_OBJECT
public:
    explicit InputManage(QObject *parent = nullptr) : QObject(parent) {}
    void update() {} // Placeholder for stick reading
};

#endif