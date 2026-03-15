// LOCATION: /home/user/ACE/src/GUI/widgets/frame.cpp
#include "widgets/frame.h"

#include <QPainter>
#include <QPainterPath>

#include "theme/theme.h"

Frame::Frame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    m_container = new QVBoxLayout(this);
    m_container->setContentsMargins(15, 15, 15, 15);
}

void Frame::set_params(float edge, float border, int alpha)
{
    m_edge = edge;
    m_border = border;
    m_alpha = alpha;
    update();
}

void Frame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    float w = width();
    float h = height();
    QPainterPath path;
    path.moveTo(m_edge, 0);
    path.lineTo(w - m_edge, 0);
    path.lineTo(w, m_edge);
    path.lineTo(w, h - m_edge);
    path.lineTo(w - m_edge, h);
    path.lineTo(m_edge, h);
    path.lineTo(0, h - m_edge);
    path.lineTo(0, m_edge);
    path.closeSubpath();

    QColor themeColor(ThemeManager::instance().active().hex);
    QColor bgColor = themeColor.darker(1000);
    bgColor.setAlpha(255);

    painter.fillPath(path, bgColor);
    painter.setPen(QPen(themeColor, m_border));
    painter.drawPath(path);
}
