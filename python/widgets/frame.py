import sys
import os
from PySide6.QtWidgets import QFrame, QVBoxLayout
from PySide6.QtGui import QPainter, QColor, QPen, QPainterPath
from PySide6.QtCore import Qt

# Ensure the widget can find theme.py
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import theme

class CustomFrame(QFrame):
    def __init__(self, parent=None, edge_size=15.0, border_width=2.0, bg_alpha=150):
        super().__init__(parent)
        
        # Ensure theme is initialized
        if not theme.ACTIVE:
            theme.set_active_theme("RED")

        self.edge_size = edge_size
        self.border_width = border_width
        self.bg_alpha = bg_alpha
        
        self._current_theme_hex = theme.ACTIVE.get("hex")
        self._bg_cache = None
        
        # Internal layout so you can add widgets easily
        self.container = QVBoxLayout(self)
        self.container.setContentsMargins(edge_size, edge_size, edge_size, edge_size)

    def addWidget(self, widget):
        self.container.addWidget(widget)

    def paintEvent(self, event):
        active_hex = theme.ACTIVE.get("hex")
        
        # Handle Theme Switching
        if self._current_theme_hex != active_hex:
            self._bg_cache = None
            self._current_theme_hex = active_hex

        p = QPainter(self)
        try:
            p.setRenderHint(QPainter.Antialiasing)
            w, h = float(self.width()), float(self.height())
            e = self.edge_size

            # 1. GENERATE GEOMETRY (The HUD "Cut Corner" Shape)
            path = QPainterPath()
            path.moveTo(e, 0)
            path.lineTo(w - e, 0)
            path.lineTo(w, e)
            path.lineTo(w, h - e)
            path.lineTo(w - e, h)
            path.lineTo(e, h)
            path.lineTo(0, h - e)
            path.lineTo(0, e)
            path.closeSubpath()

            # 2. DRAW DITHERED GRADIENT BACKGROUND
            if self._bg_cache is None or self._bg_cache.size() != self.size():
                self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

            p.setClipPath(path)
            if self._bg_cache:
                p.drawPixmap(0, 0, self._bg_cache)
            
            # 3. APPLY SEMI-TRANSPARENT OVERLAY
            # This dims the background to make text readable
            overlay = QColor(0, 0, 0, self.bg_alpha)
            p.fillPath(path, overlay)
            p.setClipping(False)

            # 4. DRAW THE THEMED BORDER
            border_color = QColor(theme.ACTIVE["hex"])
            p.setPen(QPen(border_color, self.border_width))
            p.drawPath(path)

        finally:
            # Critical Pi 4 Safety
            p.end()