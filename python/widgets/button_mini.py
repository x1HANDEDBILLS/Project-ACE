from PySide6.QtWidgets import QPushButton
from PySide6.QtGui import QPainter, QPen, QPainterPath, QFont, QColor
from PySide6.QtCore import Qt
import theme

class MiniTacticalButton(QPushButton):
    def __init__(self, text, parent=None, small=True):
        super().__init__(text, parent)
        self.setFixedSize(100, 35) if small else self.setFixedSize(160, 50)
        self._is_pressed = False

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h, e = float(self.width()), float(self.height()), 8.0
        path = QPainterPath()
        path.moveTo(e, 0); path.lineTo(w-e, 0); path.lineTo(w, e); path.lineTo(w, h-e)
        path.lineTo(w-e, h); path.lineTo(e, h); path.lineTo(0, h-e); path.lineTo(0, e); path.closeSubpath()
        overlay = QColor(theme.ACTIVE["primary"])
        overlay.setAlpha(180 if self.isDown() else 30)
        p.fillPath(path, overlay)
        p.setPen(QPen(theme.ACTIVE["primary"], 2))
        p.drawPath(path)
        p.setPen(theme.ACTIVE["primary"])
        p.setFont(QFont("Consolas", 10, QFont.Weight.Bold))
        p.drawText(self.rect(), Qt.AlignCenter, self.text().upper())