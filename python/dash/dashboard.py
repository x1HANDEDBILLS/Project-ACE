import sys
import os
from PySide6.QtWidgets import QMainWindow, QWidget, QVBoxLayout
from PySide6.QtCore import Qt
from PySide6.QtGui import QPainter

# Add parent directory to path to find theme and widgets folders
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import theme
from dash.dashboard_panel import DashboardPanel 

class Dashboard(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PROJECT ACE")
        
        # 1. Performance Cache for the background
        self._bg_cache = None
        
        # Ensure theme is ready
        if not theme.ACTIVE:
            theme.set_active_theme(theme.load_saved_hex())
        
        # 2. ROOT WIDGET & MAIN LAYOUT
        self.central = QWidget()
        self.setCentralWidget(self.central)
        
        # Margins are 0 so the DashboardPanel fills the entire screen
        self.main_layout = QVBoxLayout(self.central)
        self.main_layout.setContentsMargins(0, 0, 0, 0) 
        self.main_layout.setSpacing(0)

        # 3. ATTACH THE MAIN CONTENT PANEL
        self.panel = DashboardPanel(self)
        self.main_layout.addWidget(self.panel)
        
        self.showFullScreen()

    def refresh_theme(self):
        """ 
        SMART REFRESH: Triggered when theme colors change.
        """
        self._bg_cache = None
        self._bg_cache = theme.get_numpy_gradient(self.width(), self.height())
        
        if hasattr(self.panel, 'refresh_theme'):
            self.panel.refresh_theme()
            
        self.update()

    def attach_widget(self, widget):
        """ Utility to add extra modules if needed later """
        self.main_layout.insertWidget(self.main_layout.count() - 1, widget)

    def resizeEvent(self, event):
        self._bg_cache = theme.get_numpy_gradient(self.width(), self.height())
        super().resizeEvent(event)

    def paintEvent(self, event):
        p = QPainter(self)
        try:
            if self._bg_cache:
                p.drawPixmap(0, 0, self._bg_cache)
        finally:
            p.end() 

    def show_update(self, proc): 
        """ 
        MAIN UPDATE BRIDGE 
        This is the heartbeat of the UI.
        """
        # 1. Update the main Dashboard Panel (HUD, Hz, Signal)
        if hasattr(self, 'panel'):
            self.panel.update_panel(proc)
        
        # 2. Update the Tuning Panel specifically
        # We look for the panel inside the DashboardPanel layers
        if hasattr(self.panel, 'tuning_panel'):
            # proc.state contains the 'raw_axes' and 'tuned_axes' dict
            self.panel.tuning_panel.update_state(proc.state)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape: 
            self.close()