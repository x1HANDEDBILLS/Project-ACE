#include "theme.h"
#include "widgets/keypad.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QTimer>

Keypad::Keypad(double initial_val, QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    display = new QLineEdit();
    display->setAlignment(Qt::AlignCenter);
    display->setReadOnly(true);
    display->setFixedHeight(50);
    layout->addWidget(display);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(8);

    QStringList keys_text = {"7", "8", "9", "4", "5", "6", "1", "2", "3", ".", "0", "-", "CLR", "BACK", "CANCEL", "ENTER"};
    
    int row = 0, col = 0;
    for (const QString &txt : keys_text) {
        Button *btn = new Button(txt, this, true);
        btn->setFixedSize(70, 60);
        if (txt == "ENTER") {
            grid->addWidget(btn, row, col, 1, 2);
            btn->setFixedSize(148, 60);
        } else {
            grid->addWidget(btn, row, col);
        }
        connect(btn, &QPushButton::clicked, this, &Keypad::handle_key);
        keys.append(btn);
        col++;
        if (col > 2 && txt != "ENTER") { col = 0; row++; }
        if (txt == "ENTER") { col = 0; row++; }
    }
    layout->addLayout(grid);
    refresh_theme();
}

void Keypad::reset() {
    display->clear(); // Ensure it is blank on every open
    display->setFocus();
}

void Keypad::handle_key() {
    Button *btn = qobject_cast<Button *>(sender());
    if (!btn) return;
    QString t = btn->text();
    QString cur = display->text();

    if (t == "ENTER") emit submitted(cur.toDouble());
    else if (t == "CANCEL") emit cancelled();
    else if (t == "CLR") display->clear();
    else if (t == "BACK") display->setText(cur.left(cur.length() - 1));
    else if (t == "-") {
        if (cur.startsWith("-")) display->setText(cur.mid(1));
        else display->setText("-" + cur);
    }
    else display->insert(t);
}

void Keypad::refresh_theme() {
    QString h = ThemeManager::instance().active().hex;
    display->setStyleSheet(QString("background:#000; color:white; border:1px solid %1; font-size:24px;").arg(h));
    for (Button *b : keys) {
        if (b->text() == "ENTER") b->setStyleSheet(QString("QPushButton{background:%1; color:black; font-weight:bold;}").arg(h));
        if (b->text() == "CANCEL") b->setStyleSheet("QPushButton{background:#441111; color:white;}");
    }
}
