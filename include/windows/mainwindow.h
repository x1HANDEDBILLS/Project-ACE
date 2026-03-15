// LOCATION: /home/user/ACE/include/windows/mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>

#include "dash/dashboard_panel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

   public slots:
    void refresh_theme();
    void update_dashboard();

   private:
    void regenerate_dithered_bg();
    DashboardPanel *panel;
    QTimer *uiTimer;
    QPixmap bgCache;
};
#endif
