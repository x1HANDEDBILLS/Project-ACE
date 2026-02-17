# main.py - Core launcher + Pi optimizations

import sys
import os
import signal

from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import Qt
from PySide6.QtGui import QCursor, QGuiApplication

# Make sibling imports reliable
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# --- THEME & SAVE INTEGRATION ---
import theme
from gui.save import initialize_theme  # Critical for startup loading

# Your content layer
from gui.dashboard import DashboardContent

# Utilities
from utils.exceptions import install_global_exception_handler
from utils.window_setup import setup_window_ui
from utils.screen_scaling import apply_scaling, on_screen_changed

# Clean Ctrl+C handling
signal.signal(signal.SIGINT, signal.SIG_DFL)

# Install exception handler
install_global_exception_handler()


class GroundControlWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # 1. Apply Global Stylesheet (QSS)
        # initialize_theme() has already run in run_app(), 
        # so theme.get_qss() will now correctly return the saved color.
        self.setStyleSheet(theme.get_qss())

        # Scaling references
        self.base_width = 1024
        self.reference_dpi = 96
        self.scale_factor = 1.0

        # 2. Build UI skeleton + attach dashboard
        setup_window_ui(self)

        # 3. Initial scaling
        apply_scaling(self)

        # Pi/embedded friendly: hide cursor in fullscreen
        QApplication.instance().setOverrideCursor(QCursor(Qt.BlankCursor))

        # Watch for screen changes
        screen = self.screen() or QGuiApplication.primaryScreen()
        if screen:
            screen.geometryChanged.connect(lambda: on_screen_changed(self))
            screen.logicalDotsPerInchChanged.connect(lambda: on_screen_changed(self))

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()
        super().keyPressEvent(event)

    def closeEvent(self, event):
        QApplication.instance().restoreOverrideCursor()
        print("SYSTEM_OFFLINE // Terminal Closed")
        super().closeEvent(event)


def run_app():
    # --- 1. SYSTEM CONFIGURATION / PERSISTENT LOADS ---
    # This MUST happen before QApplication/Window creation
    initialize_theme() 
    
    # --- 2. ENVIRONMENT OPTIMIZATIONS ---
    if sys.platform == "linux" or sys.platform == "linux2":
        os.environ["QT_AUTO_SCREEN_SCALE_FACTOR"] = "0"
    
    # --- 3. APPLICATION LIFECYCLE ---
    app = QApplication(sys.argv)
    app.setStyle("Fusion") 

    window = GroundControlWindow()
    
    # OS-Based Display logic
    if sys.platform == "linux":
        window.showFullScreen()
    else:
        # Development mode for Windows
        window.resize(1024, 600)
        window.show()

    return app.exec()


if __name__ == "__main__":
    sys.exit(run_app())