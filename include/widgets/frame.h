// Location: include/widgets/frame.h
#ifndef FRAME_H
#define FRAME_H
#include <QFrame>
#include <QVBoxLayout>
class Frame : public QFrame {
    Q_OBJECT
public:
    explicit Frame(QWidget *parent = nullptr);
    void set_params(float edge, float border, int alpha);
    QVBoxLayout *container() { return m_container; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *m_container;
    float m_edge = 25.0f;
    float m_border = 2.0f;
    int m_alpha = 255;
};
#endif
