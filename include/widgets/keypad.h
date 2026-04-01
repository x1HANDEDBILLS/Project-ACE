// Location: include/widgets/keypad.h
#ifndef KEYPAD_H
#define KEYPAD_H
#include "widgets/button.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QWidget>
class Keypad : public QWidget {
    Q_OBJECT
public:
    explicit Keypad(double initial_val = 0.0, QWidget *parent = nullptr);
    void refresh_theme();
    void reset();
signals:
    void submitted(double value);
    void cancelled();
private slots:
    void handle_key();

private:
    QLineEdit *display;
    QList<Button *> keys;
};
#endif
