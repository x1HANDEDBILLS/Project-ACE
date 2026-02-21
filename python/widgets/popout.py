import random
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel
from PySide6.QtGui import QPainter, QPen, QPainterPath, QColor
from PySide6.QtCore import Qt, QTimer, QPropertyAnimation, QEasingCurve, QRect, QPoint

import theme
# Connecting to the shared broadcaster we added to animations.py
from effects.animations import TacticalGlitchEffect, GLOBAL_TELEMETRY
from widgets.signal import SignalIcon
from widgets.button_mini import MiniTacticalButton

GLITCH_MARGIN = 40

class TacticalPopout(QWidget):
    def __init__(self, parent):
        super().__init__(parent)
        self.setWindowFlags(Qt.SubWindow | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        
        self.base_w, self.base_h = 500, 320
        self.setFixedSize(self.base_w + (GLITCH_MARGIN * 2), self.base_h + (GLITCH_MARGIN * 2))
        
        self._bg_cache = None
        self._is_closing = False
        
        # Logic state (now driven by GLOBAL_TELEMETRY)
        self.signal_val = 95.0
        self.is_dropping = False
        self.is_transitioning = False
        
        # Dragging logic
        self.is_dragging = False
        self.drag_offset = QPoint()

        # Visual Effects
        self.fx = TacticalGlitchEffect()
        self.setGraphicsEffect(self.fx)
        
        # Glitch decay timer
        self.glitch_timer = QTimer(self)
        self.glitch_timer.timeout.connect(self._update_cycle)
        
        # Property animation for show/hide
        self.anim = QPropertyAnimation(self, b"geometry")

        # --- Layout Setup ---
        self.main_layout = QVBoxLayout(self)
        # Preserve your specific margins
        self.main_layout.setContentsMargins(GLITCH_MARGIN+30, GLITCH_MARGIN+15, GLITCH_MARGIN+20, GLITCH_MARGIN+20)
        
        header = QHBoxLayout()
        self.title = QLabel("// SYSTEM_READY")
        self.title.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: Consolas; font-weight: bold; font-size: 11px;")
        
        self.sig_icon = SignalIcon()
        self.sig_label = QLabel("SIGNAL: 0%")
        self.sig_label.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: Consolas; font-size: 11px; font-weight: bold;")
        
        header.addWidget(self.title)
        header.addStretch()
        header.addWidget(self.sig_icon)
        header.addWidget(self.sig_label)
        
        self.main_layout.addLayout(header)
        self.main_layout.addStretch() # Spacer for body content

        # --- The Sync Connection ---
        # Instead of a local timer, we connect to the global pulse
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)

    def _sync_telemetry(self, msg, val, dropping):
        """Verified telemetry handler synced with Settings GUI"""
        self.signal_val = val
        self.is_dropping = dropping

        if self._is_closing:
            self.title.setText("// TERMINATING...")
            self.sig_label.setText("SIGNAL: 0%")
            self.sig_icon.val = 0
        else:
            self.title.setText(msg)
            self.sig_label.setText(f"SIGNAL: {int(self.signal_val)}%")
            self.sig_icon.val = int(self.signal_val)
        
        self.sig_icon.update()

    def _update_cycle(self):
        if self.fx.intensity > 0:
            self.fx.intensity -= 1
            self.update()
        else:
            self.glitch_timer.stop()
            self.is_transitioning = False

    def show_hud(self):
        self._is_closing = False
        self.is_transitioning = True
        self.fx.intensity = 15
        self.glitch_timer.start(50)
        self.show()
        self.raise_()

    def hide_hud(self):
        self._is_closing = True
        self.is_transitioning = True
        self.fx.intensity = 25
        self.glitch_timer.start(50)
        
        curr = self.geometry()
        self.anim.stop()
        self.anim.setDuration(300)
        self.anim.setEasingCurve(QEasingCurve.InBack)
        self.anim.setStartValue(curr)
        # COLLAPSE TO MIDDLE LINE
        self.anim.setEndValue(QRect(curr.x(), curr.y() + curr.height() // 2, curr.width(), 2))
        self.anim.finished.connect(self._finalize_hide)
        self.anim.start()

    def _finalize_hide(self):
        self.hide()
        if self.parent(): self.parent().hide()
        try: self.anim.finished.disconnect(self._finalize_hide)
        except: pass

    # --- Mouse Events (Dragging Logic) ---
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.is_dragging = True
            self.drag_offset = event.position().toPoint()
            event.accept()

    def mouseMoveEvent(self, event):
        if self.is_dragging:
            # Map coordinates correctly to parent for smooth dragging
            self.move(self.mapToParent(event.position().toPoint()) - self.drag_offset)
            event.accept()

    def mouseReleaseEvent(self, event):
        self.is_dragging = False
        event.accept()

    # --- Paint Event (Background & Octagon Border) ---
    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = float(self.width()), float(self.height())
        e = 25.0
        
        path = QPainterPath()
        path.moveTo(e, 0); path.lineTo(w-e, 0); path.lineTo(w, e); path.lineTo(w, h-e)
        path.lineTo(w-e, h); path.lineTo(e, h); path.lineTo(0, h-e); path.lineTo(0, e); path.closeSubpath()
        
        if self._bg_cache is None or self._bg_cache.size() != self.size():
            self._bg_cache = theme.get_numpy_gradient(int(w), int(h))

        p.setClipPath(path)
        if self._bg_cache: p.drawPixmap(0, 0, self._bg_cache)
        p.setPen(QPen(theme.ACTIVE["primary"], 2.5))
        p.drawPath(path)