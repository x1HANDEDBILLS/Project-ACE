// Location: src/GUI/dash/input_read/input_read_page4.cpp
#include "Core/app_state.h"
#include "input_read_page4.h"
#include "theme/theme.h"
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <vector>
namespace {
class NeonBar : public QWidget {
public:
    QString label;
    int slot_index = 0;
    int16_t current_val = 0;
    NeonBar(QWidget *p) : QWidget(p) { setMinimumHeight(26); }
    void refresh() {
        // Port 4 starts at offset 150 in the 200-slot array
        int16_t new_val = AppState::instance().tuned_slots[slot_index];
        if (new_val != current_val) {
            current_val = new_val;
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor theme = ThemeManager::instance().active().primary;
        p.setBrush(QColor(12, 12, 15));
        p.setPen(QPen(QColor(40, 40, 45), 1));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 2, 2);
        float norm = (current_val + 32768.0f) / 65535.0f;
        int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));
        p.setBrush(theme);
        p.setPen(Qt::NoPen);
        p.drawRect(xPos, 4, 8, height() - 8);
        p.setPen(Qt::white);
        p.setFont(QFont("Consolas", 8, QFont::Bold));
        p.drawText(12, 0, width(), height(), Qt::AlignVCenter,
                   QString("[%1] %2: %3").arg(slot_index).arg(label).arg(current_val));
    }
};
QLabel *createHeader(const QString &txt, const QColor &color) {
    auto *l = new QLabel(txt);
    l->setStyleSheet(QString("color:%1; font-weight:bold; font-size:10px; border-bottom:1px solid "
                             "#333; margin-top:6px;")
                         .arg(color.name()));
    return l;
}
} // namespace
void setup_input_read_page4_content(QWidget *p) {
    auto *main_layout = new QVBoxLayout(p);
    main_layout->setContentsMargins(15, 10, 15, 10);
    QLabel *status = new QLabel("PORT 4 STATUS");
    status->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(status);
    auto *scroll = new QScrollArea(p);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent; border:none;");
    auto *container = new QWidget();
    auto *v_box = new QVBoxLayout(container);
    v_box->setSpacing(2);
    QColor theme = ThemeManager::instance().active().primary;
    std::vector<NeonBar *> all_bars;
    // PORT 4 ANALOG (150-157)
    v_box->addWidget(createHeader("PORT 4 ANALOG (150-157)", theme));
    for (int i = 0; i < 8; i++) {
        auto *b = new NeonBar(container);
        b->label = "AXIS " + QString::number(i);
        b->slot_index = 150 + i;
        v_box->addWidget(b);
        all_bars.push_back(b);
    }
    // PORT 4 MOTION (180-188)
    v_box->addWidget(createHeader("PORT 4 MOTION (180-188)", theme));
    QStringList m_labs = {"TRK ROLL", "TRK PITCH", "TRK YAW", "ACC X", "ACC Y", "ACC Z", "GYR X", "GYR Y", "GYR Z"};
    for (int i = 0; i < 9; i++) {
        auto *b = new NeonBar(container);
        b->label = m_labs[i];
        b->slot_index = 180 + i;
        v_box->addWidget(b);
        all_bars.push_back(b);
    }
    // PORT 4 BUTTONS (158-179)
    v_box->addWidget(createHeader("PORT 4 BUTTONS (158-179)", theme));
    auto *grid_w = new QWidget();
    auto *grid = new QGridLayout(grid_w);
    grid->setSpacing(2);
    for (int i = 0; i < 22; i++) {
        auto *l = new QLabel(QString::number(i));
        l->setFixedSize(26, 26);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("background:#080808; color:#444; border:1px solid #1a1a1a; font-size:7px;");
        grid->addWidget(l, i / 8, i % 8);
    }
    v_box->addWidget(grid_w);
    scroll->setWidget(container);
    main_layout->addWidget(scroll);
    QTimer *t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, [=]() {
        auto &state = AppState::instance();
        for (auto *b : all_bars) b->refresh();
        auto labels = grid_w->findChildren<QLabel *>();
        for (int i = 0; i < labels.size(); i++) {
            int slot = 158 + i;
            bool on = (state.tuned_slots[slot] > 0);
            labels[i]->setStyleSheet(
                on ? QString("background:%1; color:black; font-weight:bold; font-size:7px;").arg(theme.name())
                   : "background:#080808; color:#444; border:1px solid #1a1a1a; font-size:7px;");
        }
        // Dynamic status check
        bool active = (state.tuned_slots[150] != 0 || state.tuned_slots[151] != 0);
        status->setText(active ? "PORT 4: ACTIVE" : "PORT 4: WAITING");
        status->setStyleSheet(QString("color:%1; font-weight:bold;").arg(active ? theme.name() : "#444"));
    });
    t->start(16);
}
