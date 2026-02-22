from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPainter
from PySide6.QtCore import Qt
import theme

class SignalIcon(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedSize(22, 15)
        self.val = 0

    def refresh_theme(self):
        """
        Triggered by theme.py. 
        Forces the icon to re-read theme.ACTIVE during the next paint cycle.
        """
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.setPen(Qt.NoPen)
        
        # We fetch the colors directly from the ACTIVE dict during the paint loop
        primary = theme.ACTIVE.get("primary", Qt.white)
        dim = theme.ACTIVE.get("primary_dim", Qt.gray)
        
        for i in range(4):
            bar_h = (i + 1) * 3
            # Logic: If signal strength is high enough, use primary, else use dim
            p.setBrush(primary if self.val > (i * 25) else dim)
            p.drawRect(i * 5, 15 - bar_h, 3, bar_h)