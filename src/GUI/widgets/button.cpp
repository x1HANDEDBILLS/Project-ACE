// LOCATION: /home/user/ACE/src/GUI/widgets/button.cpp
#include "widgets/button.h"

#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>

#include "theme.h"

Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent), m_small(false)
{
    setFixedSize(160, 50);
}

Button::Button(const QString &text, QWidget *parent, bool small)
    : QPushButton(text, parent), m_small(small)
{
    if (m_small) {
        setFixedSize(100, 35);
        m_defaultFontSize = 9;
        m_defaultEdgeSize = 6.0f;
    } else {
        setFixedSize(160, 50);
        m_defaultFontSize = 11;
        m_defaultEdgeSize = 10.0f;
    }
}

void Button::regenerateCache()
{
    int w = width();
    int h = height();
    if (w <= 0 || h <= 0) return;
    ThemeConfig cfg = ThemeManager::instance().active();
    QImage img(w, h, QImage::Format_RGB888);
    for (int y = 0; y < h; ++y) {
        float y_ratio = (float)y / h;
        uchar *line = img.scanLine(y);
        for (int x = 0; x < w; ++x) {
            float ratio = ((float)x / w + y_ratio) / 2.0f;
            float noise = (QRandomGenerator::global()->generateDouble() - 0.5f) * 5.0f;
            int r = qBound(0,
                           (int)(cfg.bg_light.red() +
                                 (cfg.bg_dark.red() - cfg.bg_light.red()) * ratio + noise),
                           255);
            int g = qBound(0,
                           (int)(cfg.bg_light.green() +
                                 (cfg.bg_dark.green() - cfg.bg_light.green()) * ratio + noise),
                           255);
            int b = qBound(0,
                           (int)(cfg.bg_light.blue() +
                                 (cfg.bg_dark.blue() - cfg.bg_light.blue()) * ratio + noise),
                           255);
            *line++ = (uchar)r;
            *line++ = (uchar)g;
            *line++ = (uchar)b;
        }
    }
    m_bgCache = QPixmap::fromImage(img);
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    ThemeConfig cfg = ThemeManager::instance().active();

    if (m_lastThemeHex != cfg.hex || m_bgCache.isNull()) {
        m_lastThemeHex = cfg.hex;
        regenerateCache();
    }

    // Determine font size: use override if set, else use default logic
    int displayFontSize = (this->font_size != -1) ? this->font_size : m_defaultFontSize;
    QString txt = text();
    if (this->font_size == -1) {
        if (txt == "→" || txt == "⚙")
            displayFontSize = 42;
        else if (txt.contains("\n"))
            displayFontSize = 14;
        else if (txt == "INPUT")
            displayFontSize = 18;
    }

    float w = width();
    float h = height();
    float e = (this->edge_size != -1.0f) ? this->edge_size : m_defaultEdgeSize;

    // The "Cool" Tactical Path (Clipped Corners)
    QPainterPath path;
    path.moveTo(e, 0);
    path.lineTo(w - e, 0);
    path.lineTo(w, e);
    path.lineTo(w, h - e);
    path.lineTo(w - e, h);
    path.lineTo(e, h);
    path.lineTo(0, h - e);
    path.lineTo(0, e);
    path.closeSubpath();

    p.save();
    p.setClipPath(path);
    p.drawPixmap(0, 0, m_bgCache);
    p.restore();

    QColor overlay = cfg.primary;
    QColor textColor = isDown() ? QColor(255, 255, 255) : cfg.secondary;
    overlay.setAlpha(isDown() ? 220 : 35);

    p.fillPath(path, overlay);
    p.setPen(QPen(isDown() ? cfg.primary : cfg.secondary, 1.5));
    p.drawPath(path);

    p.setPen(textColor);
    p.setFont(QFont("Consolas", displayFontSize, QFont::Bold));

    QRect textRect = rect();
    textRect.moveTop(textRect.top() + y_nudge);
    p.drawText(textRect, Qt::AlignCenter, txt);
}
