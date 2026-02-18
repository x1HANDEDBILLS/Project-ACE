#ifndef LOGIC_H
#define LOGIC_H

#include <QObject>

class Logic : public QObject {
    Q_OBJECT
    Q_PROPERTY(float signalStrength READ signalStrength NOTIFY signalStrengthChanged)

public:
    explicit Logic(QObject *parent = nullptr) : QObject(parent), m_signalStrength(0.95f) {}
    
    float signalStrength() const { return m_signalStrength; }

    // ONLY the declaration here (end with a semicolon)
    bool initialize(); 
    void run();

signals:
    void signalStrengthChanged();

private:
    float m_signalStrength;
};

#endif