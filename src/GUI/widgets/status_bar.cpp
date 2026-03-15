// LOCATION: /home/user/ACE/src/GUI/widgets/status_bar.cpp
#include "status_bar.h"

#include <QHBoxLayout>

StatusBar::StatusBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    m_statusLabel = new QLabel("Disconnected", this);
    layout->addWidget(m_statusLabel);
}

void StatusBar::setConnectionStatus(bool connected)
{
    if (connected) {
        m_statusLabel->setText("Connected");
    } else {
        m_statusLabel->setText("Disconnected");
    }
}