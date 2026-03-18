// Location: src/GUI/vehicles/plane/rc_trim_plane_panel.cpp
#include "theme/theme.h"
#include "rc_trim_plane_page1.h"
#include "rc_trim_plane_panel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
RCTrimPlanePanel::RCTrimPlanePanel(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(10, 10, 10, 10);
    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();
    inner_layout->setContentsMargins(0, 0, 0, 0);
    inner_layout->setSpacing(0);
    // HEADER - Reverted to your favorite 20px layout
    QWidget *header_widget = new QWidget();
    header_widget->setAttribute(Qt::WA_StaticContents);
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    header_layout->setContentsMargins(20, 10, 20, 0);
    status_label = new QLabel("// RC TRIM: PLANE");
    header_title = new QLabel("TRIM / SUB");
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft | Qt::AlignTop);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter | Qt::AlignTop);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);
    // PAGES - Narrowed by 10px on each side
    pages = new QStackedWidget();
    pages->setFrameShape(QFrame::NoFrame);
    // This is the fix: Adding 10px margins to the LEFT and RIGHT of the page area
    pages->setContentsMargins(10, 0, 10, 0);
    page_titles = {"TRIM / SUB", "EXPO / RATES", "CALIBRATION", "LIMITS", "LOGS"};
    pages->addWidget(new RCTrimPlanePage1(this));
    for (int i = 1; i < 5; i++) {
        QWidget *p = new QWidget();
        p->setAttribute(Qt::WA_StaticContents);
        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);
    // FOOTER - Mirrored exactly
    QWidget *footer = new QWidget();
    footer->setAttribute(Qt::WA_StaticContents);
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
    connect(prev_btn, &QPushButton::clicked, this, &RCTrimPlanePanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &RCTrimPlanePanel::next_page);
    update_page_ui();
    refresh_theme();
}
void RCTrimPlanePanel::update_page_ui() {
    int idx = pages->currentIndex();
    header_title->setText(page_titles[idx]);
    page_label->setText(QString("%1 / 5").arg(idx + 1));
}
void RCTrimPlanePanel::next_page() {
    pages->setCurrentIndex((pages->currentIndex() + 1) % 5);
    update_page_ui();
}
void RCTrimPlanePanel::prev_page() {
    pages->setCurrentIndex((pages->currentIndex() - 1 + 5) % 5);
    update_page_ui();
}
void RCTrimPlanePanel::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    page_label->setStyleSheet(style + "font-size: 22px;");
    content_frame->update();
}
RCTrimPlanePanel::~RCTrimPlanePanel() {}
