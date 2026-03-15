// LOCATION: /home/user/ACE/include/widgets/keyboard.h
#ifndef TACTICAL_KEYBOARD_H
#define TACTICAL_KEYBOARD_H
#include <QGridLayout>
#include <QLineEdit>
#include <QWidget>

#include "widgets/button.h"

class TacticalKeyboard : public QWidget
{
    Q_OBJECT
   public:
    explicit TacticalKeyboard(const QString &initial_text = "", QWidget *parent = nullptr);
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
