#include "vehicles/plane/rc_map_plane_panel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "theme/theme.h"
#include "vehicles/plane/rc_map_plane_page1.h"
#include "vehicles/plane/rc_map_plane_page2.h"
#include "vehicles/plane/rc_map_plane_page3.h"
#include "vehicles/plane/rc_map_plane_page4.h"
#include "vehicles/plane/rc_map_plane_page5.h"

RCMapPlanePanel::RCMapPlanePanel(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground);

    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(30, 30, 30, 30);

    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();

    QWidget *header_widget = new QWidget();
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    status_label = new QLabel("// RC MAP: PLANE");
    header_title = new QLabel("SOURCES");
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);

    pages = new QStackedWidget();
    page_titles = {"SOURCES", "MIXING", "OUTPUTS", "FAILSAFE", "DIAGNOSTICS"};

    for (int i = 0; i < 5; ++i) {
        QWidget *p = new QWidget();

        if (i == 0) {
            setup_rc_map_plane_page1_content(p);
        } else if (i == 1) {
            // Only add layout for the new Class-based page
            QVBoxLayout *l = new QVBoxLayout(p);
            l->setContentsMargins(0, 0, 0, 0);
            RCMapPlanePage2 *pg2 = new RCMapPlanePage2(p);
            l->addWidget(pg2);
        } else if (i == 2) {
            QVBoxLayout *l = new QVBoxLayout(p);
            l->setContentsMargins(0, 0, 0, 0);
            RCMapPlanePage3 *pg3 = new RCMapPlanePage3(p);
            l->addWidget(pg3);
        } else if (i == 3)
            setup_rc_map_plane_page4_content(p);
        else if (i == 4)
            setup_rc_map_plane_page5_content(p);

        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);

    QWidget *footer = new QWidget();
    QHBoxLayout *footer_layout = new QHBoxLayout(footer);
    prev_btn = new Button("<", this, true);
    prev_btn->setFixedSize(80, 55);
    page_label = new QLabel("1 / 5");
    page_label->setAlignment(Qt::AlignCenter);
    next_btn = new Button(">", this, true);
    next_btn->setFixedSize(80, 55);
    footer_layout->addStretch();
    footer_layout->addWidget(prev_btn);
    footer_layout->addWidget(page_label);
    footer_layout->addWidget(next_btn);
    footer_layout->addStretch();
    inner_layout->addWidget(footer);

    outer_layout->addWidget(content_frame);

    connect(prev_btn, &QPushButton::clicked, this, &RCMapPlanePanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &RCMapPlanePanel::next_page);

    update_page_ui();
    refresh_theme();
}

void RCMapPlanePanel::update_page_ui()
{
    int idx = pages->currentIndex();
    header_title->setText(page_titles[idx]);
    page_label->setText(QString("%1 / 5").arg(idx + 1));
}

void RCMapPlanePanel::next_page()
{
    pages->setCurrentIndex((pages->currentIndex() + 1) % 5);
    update_page_ui();
}
void RCMapPlanePanel::prev_page()
{
    int idx = pages->currentIndex() - 1;
    if (idx < 0) idx = 4;
    pages->setCurrentIndex(idx);
    update_page_ui();
}

void RCMapPlanePanel::refresh_theme()
{
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    page_label->setStyleSheet(style + "font-size: 22px;");
    content_frame->update();
}

RCMapPlanePanel::~RCMapPlanePanel() {}
