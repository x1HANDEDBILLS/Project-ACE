// Location: src/GUI/widgets/keypad.cpp
#include "theme.h"
#include "widgets/keypad.h"
#include <QVBoxLayout>
Keypad::Keypad(double initial_val, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    display = new QLineEdit();
    display->setAlignment(Qt::AlignCenter);
    display->setReadOnly(true);
    // If value is 0, show nothing (per request)
    if (initial_val != 0.0) display->setText(QString::number(initial_val));
    layout->addWidget(display);
    QGridLayout *grid = new QGridLayout();
    QStringList keys_text = {"7", "8", "9", "4", "5", "6", "1", "2", "3", ".", "0", "CLR", "BACK", "CANCEL", "ENTER"};
    int row = 0, col = 0;
    for (const QString &txt : keys_text) {
        Button *btn = new Button(txt, this, true);
        btn->setFixedSize(70, 60);
        if (txt == "ENTER") {
            grid->addWidget(btn, row, col, 1, 2);
            btn->setFixedSize(145, 60);
        } else {
            grid->addWidget(btn, row, col);
        }
        connect(btn, &QPushButton::clicked, this, &Keypad::handle_key);
        keys.append(btn);
        col++;
        if (col > 2 && txt != "ENTER") {
            col = 0;
            row++;
        }
        if (txt == "ENTER") {
            col = 0;
            row++;
        }
    }
    layout->addLayout(grid);
    refresh_theme();
}
void Keypad::handle_key() {
    Button *btn = qobject_cast<Button *>(sender());
    if (!btn) return;
    QString t = btn->text();
    if (t == "ENTER") emit submitted(display->text().toDouble());
    else if (t == "CANCEL")
        emit cancelled();
    else if (t == "CLR")
        display->clear();
    else if (t == "BACK")
        display->setText(display->text().left(display->text().length() - 1));
    else
        display->insert(t);
}
void Keypad::refresh_theme() {
    QString h = ThemeManager::instance().active().hex;
    display->setStyleSheet(QString("background:#000; color:white; border:1px solid %1; "
                                   "font-family:'Consolas'; font-size:24px; padding:5px;")
                               .arg(h));
    for (Button *b : keys) {
        if (b->text() == "ENTER")
            b->setStyleSheet(QString("QPushButton{background:%1; color:black; font-weight:bold;}").arg(h));
        if (b->text() == "CANCEL") b->setStyleSheet("QPushButton{background:#441111; color:white;}");
    }
}
