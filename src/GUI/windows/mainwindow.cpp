// LOCATION: /home/user/ACE/src/GUI/windows/mainwindow.cpp
#include "windows/mainwindow.h"

#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <random>

#include "dash/dashboard_panel.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // 1. Set Pro Flags (No title bar, stays on top)
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // 2. Setup the Dashboard
    panel = new DashboardPanel(this);
    setCentralWidget(panel);

    // 3. UI Refresh Timer (approx 60fps)
    uiTimer = new QTimer(this);
    connect(uiTimer, &QTimer::timeout, this, &MainWindow::update_dashboard);
    uiTimer->start(16);

    setAutoFillBackground(true);

    // 4. Connect Theme Changes
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &MainWindow::refresh_theme);

    // 5. Boot Sequence: Load last used profile/theme

    // 6. Go Fullscreen
    showFullScreen();
}

MainWindow::~MainWindow() = default;

void MainWindow::regenerate_dithered_bg()
{
    int w = width();
    int h = height();
    if (w <= 0 || h <= 0) return;

    QImage img(w, h, QImage::Format_RGB888);
    ThemeConfig cfg = ThemeManager::instance().active();

    // Fixed seed for consistent dither pattern
    std::mt19937 gen(42);
    std::normal_distribution<float> dist(0.0, 1.2);

    for (int y = 0; y < h; ++y) {
        uchar *line = img.scanLine(y);
        float y_ratio = (float)y / h;
        for (int x = 0; x < w; ++x) {
            float x_ratio = (float)x / w;
            float ratio = (x_ratio + y_ratio) / 2.0f;
            float noise = dist(gen);

            int r = std::clamp(
                int(cfg.bg_light.red() + (cfg.bg_dark.red() - cfg.bg_light.red()) * ratio + noise),
                0, 255);
            int g = std::clamp(int(cfg.bg_light.green() +
                                   (cfg.bg_dark.green() - cfg.bg_light.green()) * ratio + noise),
                               0, 255);
            int b = std::clamp(int(cfg.bg_light.blue() +
                                   (cfg.bg_dark.blue() - cfg.bg_light.blue()) * ratio + noise),
                               0, 255);

            *line++ = (uchar)r;
            *line++ = (uchar)g;
            *line++ = (uchar)b;
        }
    }
    bgCache = QPixmap::fromImage(img);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (bgCache.isNull()) regenerate_dithered_bg();
    painter.drawPixmap(0, 0, bgCache);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    regenerate_dithered_bg();
}

void MainWindow::refresh_theme()
{
    regenerate_dithered_bg();
    update();
}

void MainWindow::update_dashboard()
{
    // Calls the bridge we created earlier to update controller values
    if (panel) panel->update_data();
}
