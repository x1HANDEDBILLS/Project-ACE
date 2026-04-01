// Location: src/GUI/dash/input_read/input_read_page1.cpp
#include "Core/app_state.h"
#include "dash/input_read/input_read_page1.h" // Explicit unambiguous path
#include "theme/theme.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QScroller>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>
#include <vector>
namespace {
class NeonBar : public QWidget {
    int current_val = -999999;

public:
    int id;
    QColor clr;
    NeonBar(int i, QColor c, QWidget *p) : QWidget(p), id(i), clr(c) {
        setMinimumHeight(24);
        setMinimumWidth(150);
        setAttribute(Qt::WA_OpaquePaintEvent);
    }
    void setValue(int val) {
        if (val != current_val) {
            current_val = val;
            update();
        }
    }
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor(12, 12, 15));
        p.setPen(QPen(QColor(40, 40, 45), 1));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 2, 2);
        float val = current_val;
        float norm = (val + 32768.0f) / 65536.0f;
        int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));
        p.setBrush(clr);
        p.setPen(Qt::NoPen);
        p.drawRect(xPos, 4, 8, height() - 8);
        p.setPen(QColor(70, 70, 75));
        p.drawLine(width() / 2, 2, width() / 2, height() - 2);
    }
};
struct Page1Refs {
    QLabel *valLabels[200] = {nullptr};
    QLabel *btnLabels[200] = {nullptr};
    NeonBar *neonBars[200] = {nullptr};
    int lastVals[200] = {0};
    bool lastStates[200] = {false};
};
static Page1Refs *refs = nullptr;
QWidget *createAxisRow(int id, QString name, QColor clr, QWidget *parent) {
    auto *row = new QWidget(parent);
    auto *h = new QHBoxLayout(row);
    h->setContentsMargins(10, 1, 10, 1);
    auto *lbl = new QLabel(QString("[%1] %2").arg(id, 3, 10, QChar('0')).arg(name));
    lbl->setFixedWidth(130);
    lbl->setStyleSheet("color: #aaa; font-family: 'Consolas'; font-size: 11px; border:none;");
    auto *val = new QLabel("0");
    val->setFixedWidth(65);
    val->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    val->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-size: 11px; border:none;").arg(clr.name()));
    auto *bar = new NeonBar(id, clr, row);
    if (refs) {
        refs->valLabels[id] = val;
        refs->neonBars[id] = bar;
    }
    h->addWidget(lbl);
    h->addWidget(bar, 1);
    h->addWidget(val);
    return row;
}
} // namespace
void setup_input_read_page1_content(QWidget *p) {
    if (!refs) refs = new Page1Refs();
    auto *layout = new QVBoxLayout(p);
    layout->setContentsMargins(5, 5, 5, 5);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QScroller::grabGesture(scroll->viewport(), QScroller::LeftMouseButtonGesture);
    scroll->setStyleSheet("QScrollArea { background:#050507; border:none; }");
    auto *container = new QWidget();
    auto *vbox = new QVBoxLayout(container);
    vbox->setSpacing(2);
    QVector<QColor> port_colors = {QColor("#00ffff"), QColor("#ff4444"), QColor("#00ff00"), QColor("#ffb000")};
    for (int port = 0; port < 4; port++) {
        int base = port * 50;
        auto *head = new QLabel(QString(" PORT %1 (Slots %2-%3) ").arg(port + 1).arg(base).arg(base + 49));
        head->setStyleSheet(QString("background:%1; color:#000; font-weight:bold; font-size:11px; padding:3px;")
                                .arg(port_colors[port].name()));
        vbox->addWidget(head);
        for (int i = 0; i < 16; i++)
            vbox->addWidget(createAxisRow(base + i, QString("AXIS %1").arg(i), port_colors[port], container));
        auto *grid_w = new QWidget();
        auto *grid = new QGridLayout(grid_w);
        grid->setSpacing(2);
        grid->setContentsMargins(10, 5, 10, 5);
        for (int i = 0; i < 32; i++) {
            int btn_idx = base + 16 + i;
            auto *bl = new QLabel(QString::number(btn_idx));
            bl->setFixedSize(28, 28);
            bl->setAlignment(Qt::AlignCenter);
            bl->setStyleSheet("background:#080808; color:#444; border:1px solid #1a1a1a; font-size:8px;");
            refs->btnLabels[btn_idx] = bl;
            grid->addWidget(bl, i / 8, i % 8);
        }
        vbox->addWidget(grid_w);
        for (int i = 0; i < 2; i++)
            vbox->addWidget(createAxisRow(base + 48 + i, QString("GYRO %1").arg(i), port_colors[port], container));
    }
    scroll->setWidget(container);
    layout->addWidget(scroll);
    QTimer *t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, container, [container, port_colors]() {
        auto &state = AppState::instance();
        for (int i = 0; i < 200; i++) {
            int curVal = state.raw_slots[i];
            if (std::abs(curVal - refs->lastVals[i]) > 2) {
                refs->lastVals[i] = curVal;
                if (refs->valLabels[i]) { refs->valLabels[i]->setText(QString::number(curVal)); }
                if (refs->neonBars[i]) { refs->neonBars[i]->setValue(curVal); }
            }
            if (refs->btnLabels[i]) {
                bool on = (curVal > 0);
                if (on != refs->lastStates[i]) {
                    int port = i / 50;
                    if (on) {
                        refs->btnLabels[i]->setStyleSheet(
                            QString(
                                "background:%1; color:black; font-weight:bold; border:1px solid #fff; font-size:9px;")
                                .arg(port_colors[port].name()));
                    } else {
                        refs->btnLabels[i]->setStyleSheet(
                            "background:#080808; color:#444; border:1px solid #1a1a1a; font-size:8px;");
                    }
                    refs->lastStates[i] = on;
                }
            }
        }
    });
    t->start(50);
}
InputReadPage1::InputReadPage1(QWidget *parent) : QWidget(parent) { setup_input_read_page1_content(this); }
