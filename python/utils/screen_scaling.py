from PySide6.QtGui import QFont, QGuiApplication
from PySide6.QtWidgets import QApplication

def calculate_scale_factor(window):
    screen = window.screen() or QGuiApplication.primaryScreen()
    if not screen: return window.scale_factor

    geo = screen.geometry()
    w, h = geo.width(), geo.height()
    if w < 800 or h < 500: return window.scale_factor

    dpi_scale = screen.logicalDotsPerInch() / window.reference_dpi
    res_scale = w / window.base_width
    return max(res_scale * dpi_scale, 1.15)

def apply_scaling(window):
    s = window.scale_factor
    margin = max(10, round(14 * s))
    spacing = max(8, round(10 * s))
    font_size = max(12, round(13 * s))

    # Apply global font
    QApplication.setFont(QFont("Segoe UI", font_size))

    # --- THE FIX: KEEP OUTER WRAPPERS AT ZERO MARGINS ---
    if window.centralWidget() and window.centralWidget().layout():
        window.centralWidget().layout().setContentsMargins(0, 0, 0, 0)
        window.centralWidget().layout().setSpacing(0)

    if hasattr(window, 'content_layout') and window.content_layout:
        window.content_layout.setContentsMargins(0, 0, 0, 0)
        window.content_layout.setSpacing(0)

    # Sidebars can keep internal margins
    if hasattr(window, 'sidebar_layout') and window.sidebar_layout:
        window.sidebar_layout.setContentsMargins(margin, margin, margin, margin)
        window.sidebar_layout.setSpacing(spacing)

    # Sidebar dynamic width
    screen = window.screen() or QGuiApplication.primaryScreen()
    screen_w = screen.geometry().width() if screen else 1024
    sidebar_w = round(screen_w * 0.22)
    window.sidebar.setMaximumWidth(sidebar_w)
    window.sidebar.setMinimumWidth(round(sidebar_w * 0.85))

def on_screen_changed(window):
    window.scale_factor = calculate_scale_factor(window)
    apply_scaling(window)