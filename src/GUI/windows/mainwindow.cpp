// Location: src/GUI/windows/mainwindow.cpp
#include "dashboard_panel.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"
#include "windows/mainwindow.h"
#include <QBrush>
#include <QImage>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>
#include <QTimer>
#include <algorithm>
#include <random>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Set Pro Flags
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
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MainWindow::refresh_theme);
    // 5. Initial Boot
    refresh_theme();
    // 6. Go Fullscreen
    showFullScreen();
}
MainWindow::~MainWindow() = default;
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    refresh_theme();
}
void MainWindow::refresh_theme() {
    int w = width() > 0 ? width() : 800;
    int h = height() > 0 ? height() : 480;
    QImage img(w, h, QImage::Format_RGB888);
    ThemeConfig cfg = ThemeManager::instance().active();
    // RESTORED: High-quality Gaussian Noise Generator
    std::mt19937 gen(42); // Fixed seed for consistent texture
    std::normal_distribution<float> dist(0.0, 1.2);
    for (int y = 0; y < h; ++y) {
        uchar *line = img.scanLine(y);
        float y_ratio = (float)y / h;
        for (int x = 0; x < w; ++x) {
            float x_ratio = (float)x / w;
            float ratio = (x_ratio + y_ratio) / 2.0f;
            float noise = dist(gen);
            int r =
                std::clamp(int(cfg.bg_light.red() + (cfg.bg_dark.red() - cfg.bg_light.red()) * ratio + noise), 0, 255);
            int g = std::clamp(int(cfg.bg_light.green() + (cfg.bg_dark.green() - cfg.bg_light.green()) * ratio + noise),
                               0, 255);
            int b = std::clamp(int(cfg.bg_light.blue() + (cfg.bg_dark.blue() - cfg.bg_light.blue()) * ratio + noise), 0,
                               255);
            *line++ = (uchar)r;
            *line++ = (uchar)g;
            *line++ = (uchar)b;
        }
    }
    // Set background as a static Palette brush
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(QPixmap::fromImage(img)));
    setPalette(pal);
    update();
}
void MainWindow::update_dashboard() {
    if (panel) panel->update_data();
}
