from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel
from PySide6.QtCore import Qt
import theme
from widgets.button_mini import MiniTacticalButton

class SettingsPage1(QWidget):
    def __init__(self, parent_panel):
        super().__init__()
        self.parent_panel = parent_panel
        
        # Consistent path to DashboardPanel: SettingsPanel -> Shield -> DashboardPanel
        self.dashboard = self.parent_panel.parent().parent()
        
        self.layout = QVBoxLayout(self)
        # Increased margins slightly for a better visual "buffer"
        self.layout.setContentsMargins(30, 30, 30, 30)
        self.layout.setSpacing(20)
        
        # --- ALL "PAGE 01" LABELS REMOVED ---
        
        # THEME BUTTON
        self.theme_btn = MiniTacticalButton("THEME COLOR", self)
        self.theme_btn.setFixedSize(160, 40)
        self.theme_btn.clicked.connect(self.open_theme_popout)
        self.layout.addWidget(self.theme_btn, 0, Qt.AlignLeft | Qt.AlignTop)
        
        # Pushes everything to the top
        self.layout.addStretch()

    def open_theme_popout(self):
        """Triggers the animated TacticalPopout specifically for themes"""
        if hasattr(self.dashboard, 'toggle_theme_panel'):
            self.dashboard.toggle_theme_panel()