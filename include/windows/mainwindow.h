// Location: include/windows/mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "dash/dashboard_panel.h"
#include <QMainWindow>
#include <QResizeEvent>
#include <QTimer>
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Required to ensure the background fills the screen on startup/rotation
    void resizeEvent(QResizeEvent *event) override;
public slots:
    void refresh_theme();
    void update_dashboard();

private:
    DashboardPanel *panel;
    QTimer *uiTimer;
};
#endif
