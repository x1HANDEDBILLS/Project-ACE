// Location: src/GUI/vehicles/plane/rc_map_plane_page5.cpp
#include "Core/app_state.h"
#include "protocols/nomad_daemon_link.h"
#include "theme/theme.h"
#include "rc_map_plane_page5.h"
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <vector>
namespace {
class DiagnosticsBar : public QWidget {
public:
    int ch_index = 0;
    int16_t current_raw = 0;
    int current_11bit = 0;
    DiagnosticsBar(QWidget *p) : QWidget(p) { setMinimumHeight(28); }
    void refresh() {
        auto &state = AppState::instance();
        int16_t new_raw = state.mapped_channels[ch_index];
        int new_11bit = NomadHardware::shared_channels[ch_index].load(std::memory_order_relaxed);
        if (new_raw != current_raw || new_11bit != current_11bit) {
            current_raw = new_raw;
            current_11bit = new_11bit;
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
        float norm = (current_raw + 32768.0f) / 65535.0f;
        int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));
        p.setBrush(theme);
        p.setPen(Qt::NoPen);
        p.drawRect(xPos, 4, 10, height() - 8);
        p.setPen(Qt::white);
        p.setFont(QFont("Consolas", 10, QFont::Bold));
        QString text =
            QString("CH %1 | RAW: %2 | 11-BIT OUT: %3").arg(ch_index + 1, 2).arg(current_raw, 6).arg(current_11bit, 4);
        p.drawText(12, 0, width(), height(), Qt::AlignVCenter, text);
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
    auto *main_layout = new QVBoxLayout(p);
    auto *scroll = new QScrollArea(p);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent; border:none;");
    auto *container = new QWidget();
    auto *v_box = new QVBoxLayout(container);
    v_box->setSpacing(4);
    QColor theme = ThemeManager::instance().active().primary;
    auto *header = new QLabel("HARDWARE OUTPUT: 16-BIT RAW vs 11-BIT CRSF");
    header->setStyleSheet(QString("color:%1; font-family:'Consolas'; font-weight:bold;").arg(theme.name()));
    v_box->addWidget(header);
    std::vector<DiagnosticsBar *> bars;
    for (int i = 0; i < 16; i++) {
        auto *b = new DiagnosticsBar(container);
        b->ch_index = i;
        v_box->addWidget(b);
        bars.push_back(b);
    }
    v_box->addStretch();
    scroll->setWidget(container);
    main_layout->addWidget(scroll);
    QTimer *t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, [=]() {
        for (auto *b : bars) b->refresh();
    });
    t->start(16);
}
