from PySide6.QtWidgets import QWidget, QVBoxLayout
from PySide6.QtCore import Qt
from widgets.button_mini import MiniTacticalButton
from widgets.color_picker import ColorEngineBox
from effects.animations import GLOBAL_TELEMETRY # The data source

class SettingsPage1(QWidget):
    def __init__(self, parent_panel):
        super().__init__()
        self.parent_panel = parent_panel
        
        try:
            self.dashboard = self.parent_panel.parent().parent()
        except Exception:
            self.dashboard = None
        
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(30, 30, 30, 30)
        
        self.theme_btn = MiniTacticalButton("THEME COLOR", self)
        self.theme_btn.setFixedSize(160, 40)
        self.theme_btn.clicked.connect(self.setup_theme_popout)
        self.layout.addWidget(self.theme_btn, 0, Qt.AlignLeft | Qt.AlignTop)
        
        self.layout.addStretch()

    def setup_theme_popout(self):
        """Prepares the picker and INJECTS telemetry sync before showing."""
        if not self.dashboard:
            return

        # 1. Create the picker content
        self.picker_content = ColorEngineBox()
        
        # 2. INJECTION POINT: 
        # We define a function on the fly and connect it to the Global Telemetry.
        # This ensures the Popout (the parent of picker_content) gets updated.
        def sync_hud(msg, val, dropping):
            popout = self.dashboard.theme_panel
            if popout.isVisible():
                if hasattr(popout, 'status_label'): popout.status_label.setText(msg)
                if hasattr(popout, 'sig_pct'): popout.sig_pct.setText(f"SIGNAL: {int(val)}%")
                if hasattr(popout, 'sig_icon'):
                    popout.sig_icon.val = int(val)
                    popout.sig_icon.update()

        # Connect the telemetry to this specific instance
        GLOBAL_TELEMETRY.updated.connect(sync_hud)
        
        # 3. Reference the popout and set the content
        popout = self.dashboard.theme_panel
        popout.set_content(self.picker_content, title_text="// SYSTEM_THEME_ENGINE")
        
        # 4. Show the panel
        if hasattr(self.dashboard, 'toggle_theme_panel'):
            self.dashboard.toggle_theme_panel()

    def refresh_theme(self):
        if hasattr(self.theme_btn, 'update'):
            self.theme_btn.update()