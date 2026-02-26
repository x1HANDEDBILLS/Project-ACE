from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel
from PySide6.QtCore import Qt
import theme

class TunePage1(QWidget):
    def __init__(self, parent_panel):
        super().__init__()
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(40, 60, 40, 20)
        
        self.title = QLabel("> SENSITIVITY_SETTINGS")
        self.layout.addWidget(self.title)
        self.layout.addStretch()
        self.refresh_theme()

    def update_state(self, proc):
        # Logic for live controller data goes here
        pass

    def refresh_theme(self):
        h = theme.ACTIVE['hex']
        self.title.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 18px; font-weight: bold;")