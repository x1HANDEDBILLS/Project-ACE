// Location: src/GUI/vehicles/plane/map/rc_map_plane_page5.cpp
#include "Core/app_state.h"
#include "rc_map_plane_page5.h"
#include "theme/theme.h"
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
namespace {
class DiagnosticsBar : public QWidget {
public:
    int ch_index = 0;
    int16_t current_raw = 0;
    DiagnosticsBar(QWidget *p) : QWidget(p) { setMinimumHeight(32); }
    void refresh() {
        auto &state = AppState::instance();
        // UI Fix: Pull the 'mapped_channels' which contains the Final Mixed Result
        int16_t val = state.mapped_channels[ch_index];
        if (val != current_raw) {
            current_raw = val;
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor theme = ThemeManager::instance().active().primary;
        // Background track
        p.setBrush(QColor(20, 20, 25));
        p.setPen(QPen(QColor(45, 45, 50), 1));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 4, 4);
        // Indicator math (-32768 to 32767 range)
        float norm = (current_raw + 32768.0f) / 65535.0f;
        int xPos = 2 + static_cast<int>(norm * (width() - 14));
        p.setBrush(theme);
        p.setPen(Qt::NoPen);
        p.drawRect(xPos, 6, 10, height() - 12);
        p.setPen(Qt::white);
        p.setFont(QFont("Consolas", 10, QFont::Bold));
        p.drawText(12, 0, width(), height(), Qt::AlignVCenter,
                   QString("CH %1 | FINAL MIX: %2").arg(ch_index + 1, 2).arg(current_raw, 6));
    }
};
} // namespace
void setup_rc_map_plane_page5_content(QWidget *p) {
    if (p->layout()) {
        QLayoutItem *item;
        while ((item = p->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete p->layout();
    }
    auto *main = new QVBoxLayout(p);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent; border:none;");
    auto *container = new QWidget();
    auto *v_box = new QVBoxLayout(container);
    v_box->setSpacing(6);
    for (int i = 0; i < 16; i++) {
        auto *b = new DiagnosticsBar(container);
        b->ch_index = i;
        v_box->addWidget(b);
        QTimer *t = new QTimer(b);
        QObject::connect(t, &QTimer::timeout, b, &DiagnosticsBar::refresh);
        t->start(20);
    }
    v_box->addStretch();
    scroll->setWidget(container);
    main->addWidget(scroll);
}
