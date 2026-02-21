from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPainter
from PySide6.QtCore import Qt
import theme

class SignalIcon(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedSize(22, 15)
        self.val = 0

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.setPen(Qt.NoPen)
        for i in range(4):
            bar_h = (i + 1) * 3
            p.setBrush(theme.ACTIVE["primary"] if self.val > (i * 25) else theme.ACTIVE["primary_dim"])
            p.drawRect(i * 5, 15 - bar_h, 3, bar_h)