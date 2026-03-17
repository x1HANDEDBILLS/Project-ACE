// Location: src/GUI/profiles/profile_panel.cpp
#include "Core/app_state.h"
#include "profile_manager.h"
#include "profiles/profile_panel.h"
#include "theme/theme.h"
#include "widgets/keyboard.h"
#include <QDialog>
#include <QFile>
#include <QHBoxLayout>
#include <QTimer>
#include <QVBoxLayout>
class LoadingOverlay : public QWidget {
public:
    LoadingOverlay(QWidget *parent) : QWidget(parent) {
        setFixedSize(parent->size());
        label = new QLabel("// INJECTING PARAMETERS...", this);
        label->setAlignment(Qt::AlignCenter);
        QString hex = ThemeManager::instance().active().hex;
        label->setStyleSheet(
            QString("color: %1; font-family: 'Consolas'; font-size: 20px; font-weight: bold;").arg(hex));
        QVBoxLayout *l = new QVBoxLayout(this);
        l->addWidget(label);
        setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    }
    QLabel *label;
};
ProfilePanel::ProfilePanel(QWidget *parent) : QFrame(parent) {
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
    header_widget->setAttribute(Qt::WA_StaticContents);
    QHBoxLayout *header_layout = new QHBoxLayout(header_widget);
    header_layout->setContentsMargins(20, 10, 20, 0);
    status_label = new QLabel("// STATUS: " + AppState::instance().current_profile_name);
    header_title = new QLabel("PROFILE MANAGER");
    header_title->setAlignment(Qt::AlignCenter);
    header_layout->addWidget(status_label, 1, Qt::AlignLeft | Qt::AlignTop);
    header_layout->addWidget(header_title, 2, Qt::AlignHCenter | Qt::AlignTop);
    header_layout->addStretch(1);
    inner_layout->addWidget(header_widget);
    QWidget *cat_bar = new QWidget();
    QHBoxLayout *cat_layout = new QHBoxLayout(cat_bar);
    cat_layout->setContentsMargins(20, 5, 20, 5);
    for (const QString &cat : QStringList({"PLANE", "CAR", "DRONE", "HELI"})) {
        Button *btn = new Button(cat, this, true);
        btn->setFixedSize(110, 45);
        cat_layout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, cat]() { switch_category(cat); });
    }
    inner_layout->addWidget(cat_bar);
    grid_container = new QWidget();
    grid_container->setContentsMargins(15, 0, 15, 0);
    profile_grid = new QGridLayout(grid_container);
    profile_grid->setSpacing(8);
    inner_layout->addWidget(grid_container, 1);
    outer_layout->addWidget(content_frame);
    connect(&ProfileManager::instance(), &ProfileManager::profileLoaded,
            [this]() { status_label->setText("// ACTIVE: " + AppState::instance().current_profile_name); });
    switch_category("DRONE");
    refresh_theme();
}
ProfilePanel::~ProfilePanel() {}
void ProfilePanel::switch_category(const QString &cat) {
    current_category = cat;
    header_title->setText(cat + " SLOTS");
    QLayoutItem *item;
    while ((item = profile_grid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (int i = 1; i <= 25; ++i) {
        QString name = ProfileManager::instance().get_slot_name(cat, i);
        Button *slot_btn = new Button(QString("[%1] %2").arg(i).arg(name), this, true);
        slot_btn->setFixedSize(130, 50);
        profile_grid->addWidget(slot_btn, (i - 1) / 5, (i - 1) % 5);
        connect(slot_btn, &QPushButton::clicked, [this, i, slot_btn]() { show_slot_options(i, slot_btn); });
    }
}
void ProfilePanel::show_slot_options(int slot_id, Button *btn) {
    QString current_name = ProfileManager::instance().get_slot_name(current_category, slot_id);
    QDialog dlg(this);
    dlg.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dlg.setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *l = new QVBoxLayout(&dlg);
    Frame *f = new Frame(&dlg);
    f->setFixedSize(550, 200);
    QVBoxLayout *c = f->container();
    QLabel *t = new QLabel(QString("// SLOT %1: %2").arg(slot_id).arg(current_name));
    t->setAlignment(Qt::AlignCenter);
    c->addWidget(t);
    QHBoxLayout *hb = new QHBoxLayout();
    Button *load = new Button("LOAD", &dlg, true);
    Button *save = new Button("SAVE", &dlg, true);
    Button *del = new Button("DELETE", &dlg, true);
    Button *back = new Button("BACK", &dlg, true);
    for (Button *b : {load, save, del, back}) b->setFixedHeight(60);
    hb->addWidget(load);
    hb->addWidget(save);
    hb->addWidget(del);
    hb->addWidget(back);
    c->addLayout(hb);
    l->addWidget(f);
    QString silver = ThemeManager::instance().active().secondary.name();
    t->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-weight: bold; font-size: 20px;").arg(silver));
    connect(load, &QPushButton::clicked, [this, &dlg, slot_id]() {
        trigger_loading_sequence(current_category, slot_id);
        dlg.accept();
    });
    connect(save, &QPushButton::clicked, [&]() { dlg.done(10); });
    connect(del, &QPushButton::clicked, [this, &dlg, slot_id, btn]() {
        QFile::remove(QString("profiles/%1/slot_%2.json").arg(current_category).arg(slot_id));
        btn->setText(QString("[%1] EMPTY").arg(slot_id));
        dlg.accept();
    });
    connect(back, &QPushButton::clicked, [&]() { dlg.reject(); });
    if (dlg.exec() == 10) open_keyboard(slot_id, btn);
}
void ProfilePanel::trigger_loading_sequence(const QString &cat, int slot_id) {
    LoadingOverlay *overlay = new LoadingOverlay(this);
    overlay->show();
    QTimer::singleShot(800, [this, overlay, cat, slot_id]() {
        ProfileManager::instance().load_from_category(cat, slot_id);
        overlay->deleteLater();
    });
}
void ProfilePanel::open_keyboard(int slot_id, Button *btn) {
    QString current_name = ProfileManager::instance().get_slot_name(current_category, slot_id);
    QDialog kb_dlg(this);
    kb_dlg.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    kb_dlg.setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *layout = new QVBoxLayout(&kb_dlg);
    Frame *f = new Frame(&kb_dlg);
    f->setFixedSize(720, 420);
    Keyboard *kb = new Keyboard((current_name == "EMPTY" ? "" : current_name), &kb_dlg);
    f->container()->addWidget(kb);
    layout->addWidget(f);
    connect(kb, &Keyboard::submitted, [this, &kb_dlg, slot_id, btn](QString name) {
        QString upper_name = name.toUpper();
        ProfileManager::instance().save_to_category(current_category, slot_id, upper_name);
        btn->setText(QString("[%1] %2").arg(slot_id).arg(upper_name));
        kb_dlg.accept();
        trigger_loading_sequence(current_category, slot_id);
    });
    connect(kb, &Keyboard::cancelled, [&]() { kb_dlg.reject(); });
    kb_dlg.exec();
}
void ProfilePanel::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString style = QString("color: %1; font-family: 'Consolas'; font-weight: bold;").arg(silver);
    status_label->setStyleSheet(style + "font-size: 14px;");
    header_title->setStyleSheet(style + "font-size: 32px;");
    content_frame->update();
}
