// LOCATION: /home/user/ACE/src/GUI/dash/input_read_panel.cpp
#include "dash/input_read_panel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "dash/input_read_page1.h"
#include "dash/input_read_page2.h"
#include "dash/input_read_page3.h"
#include "dash/input_read_page4.h"
#include "theme.h"

InputReadPanel::InputReadPanel(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");

    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();

    // Header
    QWidget *header_widget = new QWidget();
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    status_label = new QLabel("// SYSTEM: IO_READOUT");
    header_title = new QLabel("INPUT MONITOR");
    header_layout->addWidget(status_label, 1, Qt::AlignLeft);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);

    // Stacked Pages
    pages = new QStackedWidget();
    page_titles = {"PORT 1 TELEMETRY", "PORT 2 TELEMETRY", "PORT 3 TELEMETRY", "PORT 4 TELEMETRY"};
    for (int i = 0; i < 4; ++i) {
        QWidget *p = new QWidget();
        if (i == 0)
            setup_input_read_page1_content(p);
        else if (i == 1)
            setup_input_read_page2_content(p);
        else if (i == 2)
            setup_input_read_page3_content(p);
        else if (i == 3)
            setup_input_read_page4_content(p);
        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);

    // Footer
    QWidget *footer = new QWidget();
    QHBoxLayout *footer_layout = new QHBoxLayout(footer);
    prev_btn = new Button("<", this, true);
    prev_btn->setFixedSize(80, 55);
    page_label = new QLabel("1 / 4");
    page_label->setAlignment(Qt::AlignCenter);
    page_label->setFixedWidth(100);
    next_btn = new Button(">", this, true);
    next_btn->setFixedSize(80, 55);
    footer_layout->addStretch();
    footer_layout->addWidget(prev_btn);
    footer_layout->addWidget(page_label);
    footer_layout->addWidget(next_btn);
    footer_layout->addStretch();
    inner_layout->addWidget(footer);

    outer_layout->addWidget(content_frame);

    connect(prev_btn, &QPushButton::clicked, this, &InputReadPanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &InputReadPanel::next_page);

    update_page_ui();
    refresh_theme();
}

void InputReadPanel::update_page_ui()
{
    int idx = pages->currentIndex();
    header_title->setText(page_titles[idx]);
    page_label->setText(QString("%1 / 4").arg(idx + 1));
}
void InputReadPanel::next_page()
{
    pages->setCurrentIndex((pages->currentIndex() + 1) % 4);
    update_page_ui();
}
void InputReadPanel::prev_page()
{
    int idx = pages->currentIndex() - 1;
    if (idx < 0) idx = 3;
    pages->setCurrentIndex(idx);
    update_page_ui();
}

void InputReadPanel::refresh_theme()
{
    QString silver = ThemeManager::instance().active().secondary.name();
    status_label->setStyleSheet(QString("color: %1; font-family: 'Consolas';").arg(silver));
    header_title->setStyleSheet(
        QString("color: %1; font-size: 32px; font-weight: bold;").arg(silver));
    page_label->setStyleSheet(QString("color: %1; font-size: 22px;").arg(silver));
    content_frame->update();
}

InputReadPanel::~InputReadPanel() {}
