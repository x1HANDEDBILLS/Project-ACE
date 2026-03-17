// Location: include/widgets/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "widgets/button.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QWidget>
class Keyboard : public QWidget {
    Q_OBJECT
public:
    explicit Keyboard(const QString &initial_text = "", QWidget *parent = nullptr);
    void refresh_theme();
signals:
    void submitted(const QString &text);
    void cancelled();
private slots:
    void handle_key();

private:
    QLineEdit *display;
    QList<Button *> keys;
};
#endif
