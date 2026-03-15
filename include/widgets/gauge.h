// LOCATION: /home/user/ACE/include/widgets/gauge.h
#ifndef GAUGE_H
#define GAUGE_H

#include <QWidget>

class Gauge : public QWidget
{
    Q_OBJECT
   public:
    explicit Gauge(QWidget *parent = nullptr);
    void setValue(float value);
    void setRange(float minValue, float maxValue);
    void setLabel(const QString &text);

   protected:
    void paintEvent(QPaintEvent *event) override;

   private:
    float m_value = 0.0f;
    float m_min = 0.0f;
    float m_max = 100.0f;
    QString m_label;
};

#endif