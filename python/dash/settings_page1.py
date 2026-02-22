from PySide6.QtWidgets import QWidget, QVBoxLayout
from PySide6.QtCore import Qt
from widgets.button_mini import MiniTacticalButton
from widgets.color_picker import ColorEngineBox

class SettingsPage1(QWidget):
    def __init__(self, parent_panel):
        super().__init__()
        self.parent_panel = parent_panel
        
        # Navigation logic for GitHub architecture:
        # SettingsPage1 -> SettingsPanel -> DashboardPanel -> Dashboard
        try:
            self.dashboard = self.parent_panel.parent().parent()
        except Exception:
            self.dashboard = None
        
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(30, 30, 30, 30)
        
        # Tactical Button to launch the engine
        self.theme_btn = MiniTacticalButton("THEME COLOR", self)
        self.theme_btn.setFixedSize(160, 40)
        self.theme_btn.clicked.connect(self.setup_theme_popout)
        self.layout.addWidget(self.theme_btn, 0, Qt.AlignLeft | Qt.AlignTop)
        
        self.layout.addStretch()

    def setup_theme_popout(self):
        """Prepares and opens the popout with the Real Color Picker."""
        if not self.dashboard:
            return

        # Create the actual picker instance
        self.picker_content = ColorEngineBox()
        
        # Find the popout (theme_panel) on the Dashboard
        popout = self.dashboard.theme_panel
        
        # Inject content into the popout (Requires the updated popout.py)
        popout.set_content(self.picker_content, title_text="// SYSTEM_THEME_ENGINE")
        
        # Show the panel
        if hasattr(self.dashboard, 'toggle_theme_panel'):
            self.dashboard.toggle_theme_panel()

    def refresh_theme(self):
        """Called by SettingsPanel to update button colors live."""
        if hasattr(self.theme_btn, 'update'):
            self.theme_btn.update()