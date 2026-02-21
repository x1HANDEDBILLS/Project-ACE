import sys
import os
from PySide6.QtWidgets import QPushButton
from PySide6.QtGui import QPainter, QColor, QPen, QPainterPath, QFont
from PySide6.QtCore import Qt

# Ensure the widget can find theme.py in the parent directory
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import theme

class CustomButton(QPushButton):
    def __init__(self, text, parent=None, color_override=None, small=False):
        super().__init__(text, parent)
        
        # Ensure theme is initialized
        if not theme.ACTIVE:
            theme.set_active_theme("RED")

        # 1. FIXED COLOR INITIALIZATION
        # If color_override is a tuple (255, 0, 0), convert to QColor immediately
        self.custom_override = color_override
        if color_override:
            self.base_color = QColor(*color_override) if isinstance(color_override, tuple) else QColor(color_override)
        else:
            self.base_color = theme.ACTIVE["primary"]

        self._current_theme_hex = theme.ACTIVE.get("hex")
        self._bg_cache = None
        
        if small:
            self.setFixedSize(100, 35)
            self.font_size = 9
            self.edge_size = 6.0
        else:
            self.setFixedSize(160, 50)
            self.font_size = 11
            self.edge_size = 10.0

    def paintEvent(self, event):
        active_hex = theme.ACTIVE.get("hex")
        if self._current_theme_hex != active_hex:
            self._bg_cache = None
            self._current_theme_hex = active_hex
            if self.custom_override is None:
                self.base_color = theme.ACTIVE["primary"]

        # START PAINTER
        p = QPainter(self)
        try:
            p.setRenderHint(QPainter.Antialiasing)
            w, h = float(self.width()), float(self.height())
            e = self.edge_size

            # Path
            path = QPainterPath()
            path.moveTo(e, 0); path.lineTo(w - e, 0); path.lineTo(w, e)
            path.lineTo(w, h - e); path.lineTo(w - e, h); path.lineTo(e, h)
            path.lineTo(0, h - e); path.lineTo(0, e); path.closeSubpath()

            # Background
            if self._bg_cache is None or self._bg_cache.size() != self.size():
                self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

            p.setClipPath(path)
            if self._bg_cache:
                p.drawPixmap(0, 0, self._bg_cache)
            p.setClipping(False)

            # Interaction Overlay
            overlay = QColor(self.base_color)
            if self.isDown():
                overlay.setAlpha(220)
                text_color = QColor(255, 255, 255)
                border_color = self.base_color
            else:
                overlay.setAlpha(35)
                text_color = theme.ACTIVE["secondary"]
                border_color = theme.ACTIVE["secondary"]
                
            p.fillPath(path, overlay)

            # Border & Text
            p.setPen(QPen(border_color, 1.5))
            p.drawPath(path)
            p.setPen(text_color)
            p.setFont(QFont("Consolas", self.font_size, QFont.Weight.Bold))
            p.drawText(self.rect(), Qt.AlignCenter, self.text().upper())
            
        finally:
            # THIS IS THE CRITICAL FIX: The painter MUST end even if the code above crashes
            p.end()