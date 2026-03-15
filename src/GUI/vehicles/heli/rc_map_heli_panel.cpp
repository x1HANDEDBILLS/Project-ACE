// LOCATION: /home/user/ACE/src/GUI/vehicles/heli/rc_map_heli_panel.cpp
#include "vehicles/heli/rc_map_heli_panel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "theme/theme.h"

RCMapHeliPanel::RCMapHeliPanel(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(10, 10, 10, 10);
    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();
    inner_layout->setContentsMargins(0, 0, 0, 0);
    inner_layout->setSpacing(0);
    QWidget *header_widget = new QWidget();
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    header_layout->setContentsMargins(20, 10, 20, 0);
    status_label = new QLabel("// RC MAP: HELICOPTER");
    header_title = new QLabel("HELI SOURCE MAPPING");
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft | Qt::AlignTop);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter | Qt::AlignTop);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);
    pages = new QStackedWidget();
    page_titles = {"SOURCES", "SWASH", "TRIMS", "CURVES", "FAILSAFE"};
    for (int i = 0; i < 5; ++i) {
        QWidget *p = new QWidget();
        QVBoxLayout *pv = new QVBoxLayout(p);
        QLabel *l = new QLabel(page_titles[i] + " CONFIGURATION", p);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("color: #c0c0c0; font-family: 'Consolas'; font-size: 24px;");
        pv->addWidget(l);
        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);
    QWidget *footer = new QWidget();
    QHBoxLayout *footer_layout = new QHBoxLayout(footer);
    footer_layout->setContentsMargins(0, 0, 0, 10);
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
    connect(prev_btn, &QPushButton::clicked, this, &RCMapHeliPanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &RCMapHeliPanel::next_page);
    update_page_ui();
    refresh_theme();
}
void RCMapHeliPanel::update_page_ui()
{
    int idx = pages->currentIndex();
    header_title->setText(page_titles[idx]);
    page_label->setText(QString("%1 / 5").arg(idx + 1));
}
void RCMapHeliPanel::next_page()
{
    pages->setCurrentIndex((pages->currentIndex() + 1) % 5);
    update_page_ui();
}
void RCMapHeliPanel::prev_page()
{
    int idx = pages->currentIndex() - 1;
    if (idx < 0) idx = 4;
    pages->setCurrentIndex(idx);
    update_page_ui();
}
void RCMapHeliPanel::refresh_theme()
{
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    page_label->setStyleSheet(style + "font-size: 22px;");
    content_frame->update();
}
RCMapHeliPanel::~RCMapHeliPanel() {}
