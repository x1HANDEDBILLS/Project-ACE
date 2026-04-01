#include "input_tune_plane_page1.h"
#include "Core/app_state.h"
#include "theme/theme.h"
#include "vehicles/plane/input_tune/input_tune_plane_panel.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScroller>
#include <QTimer>
#include <QVBoxLayout>
#include <QPainter>

// --- NeonBar Implementation ---
NeonBar::NeonBar(QColor c, QWidget *p) : QWidget(p), clr(c) {
    setMinimumHeight(24);
    setMinimumWidth(100);
}

void NeonBar::setValue(int val) {
    if (val != current_val) {
        current_val = val;
        update();
    }
}

void NeonBar::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(12, 12, 15));
    p.setPen(QPen(QColor(40, 40, 45), 1));
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 2, 2);
    float norm = (current_val + 32768.0f) / 65536.0f;
    int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));
    p.setBrush(clr);
    p.setPen(Qt::NoPen);
    p.drawRect(xPos, 4, 8, height() - 8);
    p.setPen(QColor(70, 70, 75));
    p.drawLine(width() / 2, 2, width() / 2, height() - 2);
}

// --- TuneRow Implementation ---
TuneRow::TuneRow(int id, QColor page_clr, QWidget *p, InputTunePlanePanel *panel) 
    : QFrame(p), slot_id(id), parent_panel(panel) {
    auto &state = AppState::instance();
    auto &theme = ThemeManager::instance().active();
    setStyleSheet("background: #0f0f15; border: 1px solid #222; border-radius: 4px;");
    setFixedHeight(105);

    auto *main_v = new QVBoxLayout(this);
    main_v->setContentsMargins(10, 5, 10, 5);
    
    auto *mon = new QHBoxLayout();
    QLabel *id_lbl = new QLabel(QString("[%1]").arg(id, 3, 10, QChar('0')));
    id_lbl->setStyleSheet("color: #aaa; font-family: 'Consolas'; border:none;");
    
    in_bar = new NeonBar(QColor("#444"), this);
    out_bar = new NeonBar(page_clr, this);
    
    val_label = new QLabel("0");
    val_label->setFixedWidth(60);
    val_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    val_label->setStyleSheet(QString("color: %1; font-family: 'Consolas'; border:none;").arg(page_clr.name()));
    
    mon->addWidget(id_lbl); 
    mon->addWidget(in_bar, 1); 
    mon->addWidget(out_bar, 1); 
    mon->addWidget(val_label);
    main_v->addLayout(mon);

    auto *ctrls = new QHBoxLayout();
    dz_btn = new QPushButton();
    axial_btn = new QPushButton();
    expo_btn = new QPushButton();

    auto initBtn = [&](QPushButton* b, float &val) {
        b->setFixedHeight(40);
        b->setStyleSheet(QString("background:#050507; color:%1; border:1px solid #333; font-weight:bold; font-size:10px;").arg(theme.hex));
        connect(b, &QPushButton::pressed, [this, &val]() {
            QScrollArea *sa = qobject_cast<QScrollArea*>(this->parentWidget()->parentWidget());
            if (sa) QScroller::scroller(sa->viewport())->stop();
            if (parent_panel) parent_panel->open_keypad(&val, this);
        });
    };

    initBtn(dz_btn, state.deadzones[id]);
    initBtn(axial_btn, state.axial_deadzones[id]);
    initBtn(expo_btn, state.expo_values[id]);

    ctrls->addWidget(dz_btn); 
    ctrls->addWidget(axial_btn); 
    ctrls->addWidget(expo_btn);
    main_v->addLayout(ctrls);
    refresh();
}

void TuneRow::refresh() {
    auto &state = AppState::instance();
    in_bar->setValue(state.raw_slots[slot_id]);
    out_bar->setValue(state.tuned_slots[slot_id]);
    val_label->setText(QString::number(state.tuned_slots[slot_id]));
    
    dz_btn->setText(QString("DZ: %1%").arg(state.deadzones[slot_id], 0, 'f', 0));
    axial_btn->setText(QString("AXIAL: %1%").arg(state.axial_deadzones[slot_id], 0, 'f', 0));
    expo_btn->setText(QString("EXPO: %1%").arg(state.expo_values[slot_id], 0, 'f', 0));
}

void setup_input_tune_page_content(QWidget *p, int offset, InputTunePlanePanel *panel) {
    auto *layout = new QVBoxLayout(p);
    layout->setContentsMargins(5, 5, 5, 5);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QScroller::grabGesture(scroll->viewport(), QScroller::LeftMouseButtonGesture);
    scroll->setStyleSheet("QScrollArea { background:#050507; border:none; }");
    auto *container = new QWidget();
    auto *vbox = new QVBoxLayout(container);
    vbox->setSpacing(6);
    QList<TuneRow*> rows;
    for (int i = 0; i < 50; i++) {
        auto *row = new TuneRow(offset + i, QColor("#00ffff"), container, panel);
        vbox->addWidget(row); rows.append(row);
    }
    vbox->addStretch(); scroll->setWidget(container); layout->addWidget(scroll);
    QTimer *t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, [rows]() { for (auto *r : rows) { if (!r->isHidden()) r->refresh(); } });
    t->start(33);
}
