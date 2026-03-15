// LOCATION: /home/user/ACE/src/GUI/rc_type/rc_type_panel.cpp
#include "rc_type/rc_type_panel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "theme/theme.h"

RCTypePanel::RCTypePanel(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");
    page_titles = {"FIXED WING", "MULTIROTOR", "HELICOPTER", "ROVER / CAR", "SUBMERSIBLE"};
    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(10, 10, 10, 10);
    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();
    inner_layout->setContentsMargins(0, 0, 0, 0);
    QWidget *header_widget = new QWidget();
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    header_layout->setContentsMargins(20, 10, 20, 0);
    status_label = new QLabel("// CONFIGURATION ACTIVE");
    header_title = new QLabel(page_titles[0]);
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft | Qt::AlignTop);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter | Qt::AlignTop);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);
    selector = new RCTypeSelector(this);
    inner_layout->addWidget(selector);
    pages = new QStackedWidget();
    for (int i = 0; i < 5; ++i) {
        QWidget *p = new QWidget();
        QVBoxLayout *pv = new QVBoxLayout(p);
        QLabel *l = new QLabel(QString("CONFIGURE %1 PARAMETERS").arg(page_titles[i]), p);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("color: #c0c0c0; font-size: 18px; font-family: 'Consolas';");
        pv->addWidget(l);
        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);
    QWidget *footer = new QWidget();
    QHBoxLayout *footer_layout = new QHBoxLayout(footer);
    prev_btn = new Button("<", this, true);
    prev_btn->setFixedSize(80, 55);
    page_label = new QLabel("1 / 5");
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
    connect(prev_btn, &QPushButton::clicked, this, &RCTypePanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &RCTypePanel::next_page);
    refresh_theme();
}
void RCTypePanel::update_page_ui()
{
    int idx = pages->currentIndex();
    if (!page_titles.empty() && idx >= 0 && idx < page_titles.size()) {
        header_title->setText(page_titles[idx]);
    }
    page_label->setText(QString("%1 / 5").arg(idx + 1));
}
void RCTypePanel::next_page()
{
    pages->setCurrentIndex((pages->currentIndex() + 1) % 5);
    update_page_ui();
}
void RCTypePanel::prev_page()
{
    int idx = pages->currentIndex() - 1;
    if (idx < 0) idx = 4;
    pages->setCurrentIndex(idx);
    update_page_ui();
}
void RCTypePanel::refresh_theme()
{
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    page_label->setStyleSheet(style + "font-size: 22px;");
    content_frame->update();
}
