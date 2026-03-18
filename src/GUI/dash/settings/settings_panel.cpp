// Location: src/GUI/dash/settings_panel.cpp
#include "settings_base.h" // New Base
#include "settings_page1.h"
#include "settings_page2.h"
#include "settings_panel.h"
#include "theme/theme.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
SettingsPanel::SettingsPanel(QWidget *parent) : QFrame(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");
    page_titles = {"UI THEME", "CONNECTIVITY", "HARDWARE", "SECURITY", "SYSTEM INFO"};
    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(10, 10, 10, 10);
    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();
    inner_layout->setContentsMargins(0, 0, 0, 0);
    inner_layout->setSpacing(0);
    QWidget *header_widget = new QWidget();
    header_widget->setAttribute(Qt::WA_StaticContents);
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    header_layout->setContentsMargins(20, 10, 20, 0);
    status_label = new QLabel("// SYSTEM READY");
    header_title = new QLabel(page_titles[0]);
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft | Qt::AlignTop);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter | Qt::AlignTop);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);
    pages = new QStackedWidget();
    pages->setFrameShape(QFrame::NoFrame);
    pages->addWidget(new SettingsPage1(this));
    pages->addWidget(new SettingsPage2(this));
    // Fix: Using SettingsBasePage instead of raw QWidget
    for (int i = 2; i < 5; ++i) {
        SettingsBasePage *p = new SettingsBasePage(this);
        QVBoxLayout *pv = new QVBoxLayout(p);
        QLabel *l = new QLabel(page_titles[i] + " INTERFACE", p);
        l->setObjectName("placeholder_label");
        l->setAlignment(Qt::AlignCenter);
        pv->addWidget(l);
        pages->addWidget(p);
    }
    inner_layout->addWidget(pages, 1);
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
    connect(prev_btn, &QPushButton::clicked, this, &SettingsPanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &SettingsPanel::next_page);
    update_page_ui();
    refresh_theme();
}
SettingsPanel::~SettingsPanel() {}
void SettingsPanel::update_page_ui() {
    int idx = pages->currentIndex();
    header_title->setText(page_titles[idx]);
    page_label->setText(QString("%1 / 5").arg(idx + 1));
}
void SettingsPanel::next_page() {
    pages->setCurrentIndex((pages->currentIndex() + 1) % 5);
    update_page_ui();
}
void SettingsPanel::prev_page() {
    pages->setCurrentIndex((pages->currentIndex() - 1 + 5) % 5);
    update_page_ui();
}
void SettingsPanel::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    page_label->setStyleSheet(style + "font-size: 22px;");
    for (int i = 0; i < pages->count(); ++i) {
        // Every child is now a SettingsBasePage or derivative, so this is safe!
        QMetaObject::invokeMethod(pages->widget(i), "refresh_theme");
    }
    content_frame->update();
}
