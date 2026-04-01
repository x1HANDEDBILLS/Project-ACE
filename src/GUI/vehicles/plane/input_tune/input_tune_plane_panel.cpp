#include "vehicles/plane/input_tune/input_tune_plane_panel.h"
#include "input_tune_plane_page1.h"
#include "theme/theme.h"
#include "widgets/frame.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

InputTunePlanePanel::InputTunePlanePanel(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *outer_layout = new QVBoxLayout(this);
    outer_layout->setContentsMargins(10, 10, 10, 10);

    content_frame = new Frame(this);
    content_frame->set_params(25.0, 2.0, 255);
    QVBoxLayout *inner_layout = content_frame->container();

    // Header
    QWidget *header_w = new QWidget();
    QHBoxLayout *header_l = new QHBoxLayout(header_w);
    status_label = new QLabel("// SYSTEM: TUNING");
    header_title = new QLabel("INPUT CONDITIONING");
    header_l->addWidget(status_label, 1);
    header_l->addWidget(header_title, 2, Qt::AlignCenter);
    header_l->addStretch(1);
    inner_layout->addWidget(header_w);

    stack = new QStackedWidget();
    page_titles = {"SLOTS 000-049", "SLOTS 050-099", "SLOTS 100-149", "SLOTS 150-199"};
    for (int i = 0; i < 4; ++i) {
        QWidget *p = new QWidget();
        setup_input_tune_page_content(p, i * 50, this); 
        stack->addWidget(p);
    }
    inner_layout->addWidget(stack, 1);

    // Footer
    QWidget *footer = new QWidget();
    QHBoxLayout *footer_l = new QHBoxLayout(footer);
    prev_btn = new Button("<", this, true);
    page_label = new QLabel("1 / 4");
    next_btn = new Button(">", this, true);
    footer_l->addStretch(); footer_l->addWidget(prev_btn); footer_l->addWidget(page_label); footer_l->addWidget(next_btn); footer_l->addStretch();
    inner_layout->addWidget(footer);
    outer_layout->addWidget(content_frame);

    // Overlay Shield (Dimmer)
    keypad_shield = new QFrame(this);
    keypad_shield->setStyleSheet("background: rgba(0,0,0,200); border:none;");
    keypad_shield->hide();
    
    // THE OCTAGONAL FRAME WRAPPER
    Frame *kp_frame = new Frame(keypad_shield);
    kp_frame->set_params(20.0, 2.0, 255);
    kp_frame->setFixedSize(320, 500);
    
    QVBoxLayout *s_layout = new QVBoxLayout(keypad_shield);
    s_layout->addWidget(kp_frame, 0, Qt::AlignCenter);

    // Keypad inside the octagonal frame
    overlay_keypad = new Keypad(0.0, kp_frame);
    kp_frame->container()->addWidget(overlay_keypad);

    connect(overlay_keypad, &Keypad::submitted, [this](double val) {
        if (active_target) *active_target = (float)val;
        // INSTANT UPDATE: Tell the row to refresh its buttons NOW
        if (active_row) active_row->refresh();
        keypad_shield->hide();
    });
    connect(overlay_keypad, &Keypad::cancelled, [this]() { keypad_shield->hide(); });
    connect(prev_btn, &QPushButton::clicked, this, &InputTunePlanePanel::prev_page);
    connect(next_btn, &QPushButton::clicked, this, &InputTunePlanePanel::next_page);
}

void InputTunePlanePanel::open_keypad(float *target, TuneRow *row) {
    active_target = target;
    active_row = row;
    overlay_keypad->reset(); // Fix 1: Clear the display so it's blank on open
    keypad_shield->setGeometry(content_frame->geometry());
    keypad_shield->show();
    keypad_shield->raise();
}

InputTunePlanePanel::~InputTunePlanePanel() {}
void InputTunePlanePanel::next_page() { stack->setCurrentIndex((stack->currentIndex() + 1) % 4); update_page_ui(); }
void InputTunePlanePanel::prev_page() { stack->setCurrentIndex((stack->currentIndex() - 1 + 4) % 4); update_page_ui(); }
void InputTunePlanePanel::update_page_ui() {
    header_title->setText(page_titles[stack->currentIndex()]);
    page_label->setText(QString("%1 / 4").arg(stack->currentIndex() + 1));
}
void InputTunePlanePanel::refresh_theme() {}
