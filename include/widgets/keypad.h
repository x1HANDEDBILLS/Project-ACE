// LOCATION: /home/user/ACE/include/widgets/keypad.h
#ifndef TACTICAL_KEYPAD_H
#define TACTICAL_KEYPAD_H
#include <QGridLayout>
#include <QLineEdit>
#include <QWidget>

#include "widgets/button.h"

class TacticalKeypad : public QWidget
{
    Q_OBJECT
   public:
    explicit TacticalKeypad(double initial_val = 0.0, QWidget *parent = nullptr);
    void refresh_theme();
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
