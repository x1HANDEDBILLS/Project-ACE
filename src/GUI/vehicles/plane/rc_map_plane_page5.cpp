#include "vehicles/plane/rc_map_plane_page5.h"

#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <vector>

#include "Core/app_state.h"
#include "theme/theme.h"

namespace
{
// Custom widget to match your Input Read Page 1 aesthetic
class DiagnosticsBar : public QWidget
{
   public:
    int ch_index = 0;
    int16_t current_val = 0;

    DiagnosticsBar(QWidget* p) : QWidget(p)
    {
        setMinimumHeight(28);
    }

    void refresh()
    {
        // Mapping to the Output Channels for Page 5
        int16_t new_val = AppState::instance().mapped_channels[ch_index];
        if (new_val != current_val) {
            current_val = new_val;
            update();
        }
    }

   protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor theme = ThemeManager::instance().active().primary;

        // Background track
        p.setBrush(QColor(12, 12, 15));
        p.setPen(QPen(QColor(40, 40, 45), 1));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 2, 2);

        // Map -32768/32767 to bar width
        // This creates the "Neon" slider block look (10px width)
        float norm = (current_val + 32768.0f) / 65535.0f;
        int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));

        // Draw the sliding accent block
        p.setBrush(theme);
        p.setPen(Qt::NoPen);
        p.drawRect(xPos, 4, 10, height() - 8);

        // White text in Consolas for that tactical feel
        p.setPen(Qt::white);
        p.setFont(QFont("Consolas", 9, QFont::Bold));
        QString text = QString("CH %1: %2").arg(ch_index + 1).arg(current_val);
        p.drawText(12, 0, width(), height(), Qt::AlignVCenter, text);
    }
};
}  // namespace

void setup_rc_map_plane_page5_content(QWidget* p)
{
    if (p->layout()) delete p->layout();

    auto* main_layout = new QVBoxLayout(p);
    main_layout->setContentsMargins(10, 5, 10, 5);

    auto* scroll = new QScrollArea(p);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent; border:none;");

    auto* container = new QWidget();
    auto* v_box = new QVBoxLayout(container);
    v_box->setSpacing(4);
    v_box->setContentsMargins(0, 0, 15, 0);  // Padding for scrollbar

    QColor theme = ThemeManager::instance().active().primary;

    // Header
    auto* header = new QLabel("OUTPUT CHANNEL DIAGNOSTICS");
    header->setStyleSheet(
        QString("color:%1; font-weight:bold; font-family:'Consolas'; font-size:11px; "
                "border-bottom:1px solid #333; margin-bottom:5px;")
            .arg(theme.name()));
    v_box->addWidget(header);

    std::vector<DiagnosticsBar*> all_bars;
    for (int i = 0; i < 16; i++) {
        auto* b = new DiagnosticsBar(container);
        b->ch_index = i;
        v_box->addWidget(b);
        all_bars.push_back(b);
    }
    v_box->addStretch();

    scroll->setWidget(container);
    main_layout->addWidget(scroll);

    // High speed refresh (60fps)
    QTimer* t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, [=]() {
        for (auto* b : all_bars) b->refresh();
    });
    t->start(16);
}
