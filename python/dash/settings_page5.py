from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel
import theme

class SettingsPage5(QWidget):
    def __init__(self, parent_panel):
        super().__init__()
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(40, 60, 40, 20)
        self.title = QLabel("> SYSTEM_DIAGNOSTIC_SKELETON")
        self.layout.addWidget(self.title)
        self.layout.addStretch()
        self.refresh_theme()

    def refresh_theme(self):
        h = theme.ACTIVE['hex']
        self.title.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 18px; font-weight: bold;")