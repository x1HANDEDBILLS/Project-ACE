from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPainter, QPen, QFont
from PySide6.QtCore import Qt, QRectF, QPointF
import theme

class Gauge(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.value = 0
        self.setMinimumSize(220, 220)

    def set_value(self, val):
        self.value = val
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        cx, cy = self.width()/2, self.height()/2
        p.translate(cx, cy)
        
        # Ticks
        p.setPen(QPen(theme.ACTIVE["secondary"], 1))
        for i in range(0, 360, 10):
            p.save()
            p.rotate(i)
            p.drawLine(QPointF(90, 0), QPointF(96, 0))
            p.restore()

        # Active Arc
        rect = QRectF(-85, -85, 170, 170)
        p.setPen(QPen(theme.ACTIVE["primary"], 6))
        span = int((self.value / 100.0) * 360)
        # PySide6 drawArc uses 1/16th degrees
        p.drawArc(rect, 90 * 16, -span * 16)
        
        p.setPen(Qt.white)
        p.setFont(QFont("Consolas", 28, QFont.Bold))
        p.drawText(rect, Qt.AlignCenter, f"{int(self.value)}")