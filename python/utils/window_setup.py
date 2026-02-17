from PySide6.QtWidgets import QWidget, QHBoxLayout, QVBoxLayout
from gui.dashboard import DashboardContent

def setup_window_ui(window):
    window.setWindowTitle("RC Ground Control")

    central = QWidget()
    window.setCentralWidget(central)

    # Main structure: Sidebar + Content
    main_layout = QHBoxLayout(central)
    main_layout.setContentsMargins(0, 0, 0, 0)
    main_layout.setSpacing(0)

    # Sidebar Panel
    window.sidebar = QWidget()
    window.sidebar_layout = QVBoxLayout(window.sidebar)
    window.sidebar_layout.setContentsMargins(0, 0, 0, 0)
    window.sidebar_layout.setSpacing(0)
    main_layout.addWidget(window.sidebar)
    
    # Hide sidebar initially so the dashboard fills the whole screen
    window.sidebar.hide()

    # Main Content Area
    window.content_area = QWidget()
    window.content_layout = QVBoxLayout(window.content_area)
    window.content_layout.setContentsMargins(0, 0, 0, 0)
    window.content_layout.setSpacing(0)
    main_layout.addWidget(window.content_area, stretch=1)

    # Attach Dashboard
    # We create the widget and add it directly to the layout.
    # This replaces the .setup() call that was causing the AttributeError.
    window.dashboard = DashboardContent(window)
    window.content_layout.addWidget(window.dashboard)