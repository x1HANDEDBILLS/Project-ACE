// LOCATION: /home/user/ACE/src/GUI/dash/input_read_page5.cpp
#include "dash/input_read_page5.h"

InputReadPage5::InputReadPage5(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("SYSTEM DEBUG\n[PLACEHOLDER CONTENT]");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "color: #444; font-family: 'Consolas'; font-size: 18px; border: 1px dashed #333;");
    layout->addWidget(label);
}
