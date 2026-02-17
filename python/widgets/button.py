class Button(QPushButton):
    def __init__(self, text, parent=None):
        super().__init__(text, parent)
        self.setMinimumHeight(35)
        self._bg_cache = None
        self._current_theme_hex = None # Track the theme state

    def paintEvent(self, event):
        # --- THE FIX: Detect Theme Change ---
        # If the hex code in theme.py changes, we MUST dump the old background
        active_hex = theme.ACTIVE.get("hex")
        if self._current_theme_hex != active_hex:
            self._bg_cache = None
            self._current_theme_hex = active_hex

        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = float(self.width()), float(self.height())
        n = 8.0 # The corner cut size

        # 1. Create the Clip Path (The 45 degree corners)
        path = QPainterPath()
        path.moveTo(n, 0)
        path.lineTo(w-n, 0)
        path.lineTo(w, n)
        path.lineTo(w, h-n)
        path.lineTo(w-n, h)
        path.lineTo(n, h)
        path.lineTo(0, h-n)
        path.lineTo(0, n)
        path.closeSubpath()

        # 2. Handle Gradient Background
        if self._bg_cache is None or self._bg_cache.size() != self.size():
            self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

        p.setClipPath(path)
        p.drawPixmap(0, 0, self._bg_cache)
        p.setClipping(False)

        # 3. Dynamic Overlay (Interaction Highlights)
        # Uses ACTIVE["primary"] color
        overlay = QColor(theme.ACTIVE["primary"])
        alpha = 200 if self.isDown() else (80 if self.underMouse() else 35)
        overlay.setAlpha(alpha)
        p.fillPath(path, overlay)

        # 4. Tactical Border (The Outline)
        # Use Secondary color for idle, Primary for Hover
        border_color = theme.ACTIVE["primary"] if self.underMouse() else theme.ACTIVE["secondary"]
        p.setPen(QPen(border_color, 1.5))
        p.drawPath(path)
        
        # 5. Text Rendering
        text_color = Qt.white if self.underMouse() else theme.ACTIVE["secondary"]
        p.setPen(text_color)
        p.setFont(QFont("Consolas", 10, QFont.Bold))
        # Nudge text slightly to center better with the cuts
        p.drawText(self.rect(), Qt.AlignCenter, self.text().upper())