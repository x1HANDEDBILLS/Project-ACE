// Location: include/widgets/status_bar.h
#ifndef STATUS_BAR_H
#define STATUS_BAR_H
#include <QLabel>
#include <QWidget>
class StatusBar : public QWidget {
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);
    void setConnectionStatus(bool connected);

private:
    QLabel *m_statusLabel;
};
#endif