// Location: src/GUI/widgets/gauge.cpp
#include "gauge.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
Gauge::Gauge(QWidget *parent) : QWidget(parent) { setMinimumSize(100, 100); }
void Gauge::setValue(float value) {
    if (value < m_min) value = m_min;
    if (value > m_max) value = m_max;
    if (m_value == value) return;
    m_value = value;
    update();
}
void Gauge::setRange(float minValue, float maxValue) {
    m_min = minValue;
    m_max = maxValue;
    update();
}
void Gauge::setLabel(const QString &text) {
    m_label = text;
    update();
}
void Gauge::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int w = width();
    int h = height();
    int size = qMin(w, h);
    p.translate(w / 2, h / 2);
    p.scale(size / 200.0, size / 200.0);
    // Background
    p.setBrush(QColor(40, 40, 40));
    p.drawEllipse(-90, -90, 180, 180);
    // Value Arc
    float normalized = (m_value - m_min) / (m_max - m_min);
    p.setPen(QPen(Qt::cyan, 10));
    p.drawArc(-80, -80, 160, 160, -30 * 16, (240 * normalized) * 16);
    // Label
    p.setPen(Qt::white);
    p.drawText(-40, 40, 80, 20, Qt::AlignCenter, m_label);
}