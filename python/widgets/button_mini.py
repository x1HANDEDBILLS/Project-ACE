from PySide6.QtWidgets import QPushButton
from PySide6.QtGui import QPainter, QPen, QPainterPath, QFont, QColor
from PySide6.QtCore import Qt
import theme

class MiniTacticalButton(QPushButton):
    def __init__(self, text, parent=None, small=True):
        super().__init__(text, parent)
        
        # 1. Initialize Theme Tracking
        self._current_theme_hex = theme.ACTIVE.get("hex")
        self._bg_cache = None
        
        # 2. Geometry Config
        if small:
            self.setFixedSize(100, 35)
            self.font_size = 9
            self.edge_size = 6.0
        else:
            self.setFixedSize(160, 50)
            self.font_size = 11
            self.edge_size = 10.0

    def paintEvent(self, event):
        # 3. Theme Sync Check (Same as CustomButton)
        active_hex = theme.ACTIVE.get("hex")
        if self._current_theme_hex != active_hex:
            self._bg_cache = None
            self._current_theme_hex = active_hex

        p = QPainter(self)
        try:
            p.setRenderHint(QPainter.Antialiasing)
            w, h = float(self.width()), float(self.height())
            e = self.edge_size

            # Create the HUD notched path
            path = QPainterPath()
            path.moveTo(e, 0); path.lineTo(w-e, 0); path.lineTo(w, e)
            path.lineTo(w, h-e); path.lineTo(w-e, h); path.lineTo(e, h)
            path.lineTo(0, h-e); path.lineTo(0, e); path.closeSubpath()

            # 4. Background Gradient (Numpy Cache)
            if self._bg_cache is None or self._bg_cache.size() != self.size():
                self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

            p.setClipPath(path)
            if self._bg_cache:
                p.drawPixmap(0, 0, self._bg_cache)
            p.setClipping(False)

            # 5. Interaction Styles
            if self.isDown():
                overlay_color = QColor(theme.ACTIVE["primary"])
                overlay_color.setAlpha(220)
                text_color = QColor(255, 255, 255)
                border_color = theme.ACTIVE["primary"]
            else:
                overlay_color = QColor(theme.ACTIVE["primary"])
                overlay_color.setAlpha(35)
                text_color = theme.ACTIVE["secondary"]
                border_color = theme.ACTIVE["secondary"]

            p.fillPath(path, overlay_color)
            p.setPen(QPen(border_color, 1.5))
            p.drawPath(path)

            # 6. Text Rendering
            p.setPen(text_color)
            p.setFont(QFont("Consolas", self.font_size, QFont.Weight.Bold))
            p.drawText(self.rect(), Qt.AlignCenter, self.text().upper())
            
        finally:
            p.end()