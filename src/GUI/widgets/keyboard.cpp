// Location: src/GUI/widgets/keyboard.cpp
#include "theme.h"
#include "widgets/keyboard.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
Keyboard::Keyboard(const QString &initial_text, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    display = new QLineEdit(initial_text);
    display->setAlignment(Qt::AlignCenter);
    display->setReadOnly(true);
    layout->addWidget(display);
    const QStringList rows[] = {{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
                                {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
                                {"A", "S", "D", "F", "G", "H", "J", "K", "L", "-"},
                                {"Z", "X", "C", "V", "B", "N", "M", ".", "_"},
                                {"CLR", "SPACE", "BACK", "CANCEL", "ENTER"}};
    for (int r = 0; r < 5; ++r) {
        QHBoxLayout *row_layout = new QHBoxLayout();
        for (const QString &key : rows[r]) {
            Button *btn = new Button(key, this, true);
            if (key == "SPACE") btn->setFixedSize(140, 45);
            else if (key == "ENTER" || key == "BACK" || key == "CANCEL")
                btn->setFixedSize(80, 45);
            else
                btn->setFixedSize(45, 45);
            connect(btn, &QPushButton::clicked, this, &Keyboard::handle_key);
            keys.append(btn);
            row_layout->addWidget(btn);
        }
        layout->addLayout(row_layout);
    }
    refresh_theme();
}
void Keyboard::handle_key() {
    Button *btn = qobject_cast<Button *>(sender());
    QString key = btn->text();
    if (key == "ENTER") emit submitted(display->text());
    else if (key == "CANCEL")
        emit cancelled();
    else if (key == "BACK")
        display->setText(display->text().left(display->text().length() - 1));
    else if (key == "CLR")
        display->clear();
    else if (key == "SPACE")
        display->insert(" ");
    else
        display->insert(key);
}
void Keyboard::refresh_theme() {
    QString h = ThemeManager::instance().active().hex;
    display->setStyleSheet(QString("background: #000; color: white; border: 1px solid %1; "
                                   "font-family: 'Consolas'; font-size: 20px; padding: 5px;")
                               .arg(h));
    for (Button *b : keys) {
        if (b->text() == "ENTER") b->setStyleSheet(QString("QPushButton{background:%1; color:black;}").arg(h));
        if (b->text() == "CANCEL") b->setStyleSheet("QPushButton{background:#441111; color:white;}");
    }
}
