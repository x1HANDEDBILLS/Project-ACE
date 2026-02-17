from PySide6.QtWidgets import QFrame
from PySide6.QtGui import QPainter, QPainterPath, QPen
from PySide6.QtCore import Qt, QRectF, QPointF
import theme

class Frame(QFrame):
    def __init__(self, title="NODE_SYS", parent=None):
        super().__init__(parent)
        self.title = title
        self._bg_cache = None

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = float(self.width()), float(self.height())
        n = 20.0 

        if self._bg_cache is None or self._bg_cache.size() != self.size():
            self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

        path = QPainterPath()
        path.moveTo(n, 0); path.lineTo(w-n, 0); path.lineTo(w, n)
        path.lineTo(w, h-n); path.lineTo(w-n, h); path.lineTo(n, h)
        path.lineTo(0, h-n); path.lineTo(0, n); path.closeSubpath()

        p.setClipPath(path)
        p.drawPixmap(0, 0, self._bg_cache)
        p.setClipping(False)

        p.setPen(QPen(theme.ACTIVE["secondary"], 1))
        p.drawPath(path)
        
        p.setPen(QPen(theme.ACTIVE["primary"], 2))
        p.drawLine(QPointF(n, 0), QPointF(0, n))
        p.drawText(QRectF(25, 8, 200, 20), Qt.AlignLeft, f"// {self.title}")