// Location: src/GUI/vehicles/plane/rc_trim_plane_page1.cpp
#include "Core/app_state.h"
#include "theme/theme.h"
#include "vehicles/plane/rc_trim_plane_page1.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QScrollerProperties>
#include <QVBoxLayout>
#include <vector>
namespace {
QLabel *createSectionHeader(const QString &text) {
    auto *l = new QLabel(text);
    l->setStyleSheet("color: white; font-weight: bold; font-size: 16px; background: #050505; "
                     "border-bottom: 2px solid #333; padding: 5px; margin-top: 10px;");
    l->setAttribute(Qt::WA_StaticContents);
    return l;
}
} // namespace
RCTrimPlanePage1::RCTrimPlanePage1(QWidget *parent) : QWidget(parent) {
    auto *main_layout = new QVBoxLayout(this);
    // ADJUSTMENT: This creates the "10px shorter on both sides" look
    // without affecting the header/footer of the parent panel.
    main_layout->setContentsMargins(10, 0, 10, 0);
    auto &theme = ThemeManager::instance().active();
    QString themeColor = theme.primary.name();
    // 1. Step Header
    auto *top_bar = new QHBoxLayout();
    top_bar->setContentsMargins(10, 5, 10, 5);
    QLabel *step_label = new QLabel("TRIM STEP:");
    step_label->setStyleSheet("color: #888; font-weight: bold; font-size: 14px;");
    QPushButton *step_toggle = new QPushButton("1%");
    step_toggle->setFixedSize(100, 45);
    step_toggle->setStyleSheet(QString("background: #111; color: %1; border: 2px solid %1; "
                                       "border-radius: 8px; font-weight: bold; font-size: 18px;")
                                   .arg(themeColor));
    static int current_step = 1;
    std::vector<int> steps = {1, 2, 4, 6, 10};
    static int step_idx = 0;
    connect(step_toggle, &QPushButton::clicked, [=]() mutable {
        step_idx = (step_idx + 1) % (int)steps.size();
        current_step = steps[step_idx];
        step_toggle->setText(QString("%1%").arg(current_step));
    });
    top_bar->addWidget(step_label);
    top_bar->addWidget(step_toggle);
    top_bar->addStretch();
    main_layout->addLayout(top_bar);
    // 2. THE SCROLL AREA
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->viewport()->setAttribute(Qt::WA_StaticContents);
    scroll->setAutoFillBackground(true);
    QPalette p = scroll->palette();
    p.setColor(QPalette::Window, QColor(5, 10, 15));
    scroll->setPalette(p);
    scroll->setStyleSheet(
        QString("QScrollArea { background: #050a0f; border: none; } "
                "QScrollBar:vertical { background: #000; width: 14px; margin: 0px; } "
                "QScrollBar::handle:vertical { background: %1; min-height: 40px; border-radius: 7px; } "
                "QScrollBar::add-line, QScrollBar::sub-line { height: 0px; } ")
            .arg(themeColor));
    QScroller *scroller = QScroller::scroller(scroll->viewport());
    scroller->grabGesture(scroll->viewport(), QScroller::LeftMouseButtonGesture);
    auto *container = new QWidget();
    container->setAutoFillBackground(true);
    container->setPalette(p);
    auto *v_box = new QVBoxLayout(container);
    v_box->setContentsMargins(5, 10, 5, 10);
    v_box->setSpacing(10);
    auto create_tune_card = [&](int i, int16_t *val_ptr, QString color, QString label) {
        QFrame *card = new QFrame();
        // ADJUSTMENT: Reduced width from 360 to 350 to accommodate the new 10px page margins
        card->setFixedSize(350, 110);
        card->setStyleSheet("background: #0a0a0a; border: 2px solid #222; border-radius: 10px;");
        auto *l = new QVBoxLayout(card);
        l->setSpacing(5);
        auto *title = new QLabel(QString("CH %1 %2").arg(i + 1).arg(label));
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("color: white; font-size: 14px; font-weight: bold; "
                             "background: transparent; border: none;");
        l->addWidget(title);
        auto *ctrl_row = new QHBoxLayout();
        QPushButton *val_btn = new QPushButton(QString::number(*val_ptr));
        val_btn->setFixedSize(75, 42);
        val_btn->setStyleSheet(QString("background: #000; color: white; border: 1px solid %1; "
                                       "border-radius: 4px; font-family: 'Consolas'; font-size: 16px;")
                                   .arg(color));
        auto setup_btn = [&](QString txt, int dir) {
            QPushButton *b = new QPushButton(txt);
            b->setFixedSize(65, 55);
            b->setStyleSheet(QString("background: #181818; color: %1; border-radius: 8px; "
                                     "font-size: 24px; font-weight: bold;")
                                 .arg(color));
            b->setAutoRepeat(true);
            connect(b, &QPushButton::clicked, [=]() {
                *val_ptr += (dir * current_step);
                val_btn->setText(QString::number(*val_ptr));
            });
            return b;
        };
        ctrl_row->addWidget(setup_btn("-", -1));
        ctrl_row->addWidget(val_btn);
        ctrl_row->addWidget(setup_btn("+", 1));
        l->addLayout(ctrl_row);
        return card;
    };
    v_box->addWidget(createSectionHeader("FLIGHT TRIMS"));
    for (int i = 0; i < 16; i += 2) {
        auto *row = new QHBoxLayout();
        row->setSpacing(10);
        row->addWidget(create_tune_card(i, &AppState::instance().physical_trims[i], "#00ffff", "TRIM"));
        row->addWidget(create_tune_card(i + 1, &AppState::instance().physical_trims[i + 1], "#00ffff", "TRIM"));
        v_box->addLayout(row);
    }
    v_box->addWidget(createSectionHeader("TRIM ALIGNMENT"));
    for (int i = 0; i < 16; i += 2) {
        auto *row = new QHBoxLayout();
        row->setSpacing(10);
        row->addWidget(create_tune_card(i, &AppState::instance().sub_trims[i], "#ffb000", "ALIGN"));
        row->addWidget(create_tune_card(i + 1, &AppState::instance().sub_trims[i + 1], "#ffb000", "ALIGN"));
        v_box->addLayout(row);
    }
    scroll->setWidget(container);
    main_layout->addWidget(scroll);
}
void RCTrimPlanePage1::refresh_ui() {}
