import random
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel
from PySide6.QtGui import QPainter, QPen, QPainterPath, QColor
from PySide6.QtCore import Qt, QTimer, QPropertyAnimation, QEasingCurve, QRect, QPoint

import theme
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
        self.signal_val = 95.0
        self.is_dropping = False
        self.is_transitioning = False
        
        self.is_dragging = False
        self.drag_offset = QPoint()

        self.fx = TacticalGlitchEffect()
        self.setGraphicsEffect(self.fx)
        self.glitch_timer = QTimer(self)
        self.glitch_timer.timeout.connect(self._update_cycle)
        self.anim = QPropertyAnimation(self, b"geometry")

        # --- Layout Setup ---
        self.main_layout = QVBoxLayout(self)
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
        
        self.main_layout.addStretch() 

        # --- RESTORED CLOSE BUTTON ---
        self.close_btn = MiniTacticalButton("EXIT", self, small=True)
        self.close_btn.setFixedSize(90, 35)
        self.close_btn.clicked.connect(self.hide_hud)
        self.main_layout.addWidget(self.close_btn, 0, Qt.AlignRight | Qt.AlignBottom)

        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)

    def _sync_telemetry(self, msg, val, dropping):
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
        
        # Notify shield to hide if applicable
        if self.parent() and hasattr(self.parent(), 'hide'):
             # We let the animation finish before hiding the shield entirely 
             # via the smart_back logic in DashboardPanel
             pass

        curr = self.geometry()
        self.anim.stop()
        self.anim.setDuration(300)
        self.anim.setEasingCurve(QEasingCurve.InBack)
        self.anim.setStartValue(curr)
        self.anim.setEndValue(QRect(curr.x(), curr.y() + curr.height() // 2, curr.width(), 2))
        self.anim.finished.connect(self._finalize_hide)
        self.anim.start()

    def _finalize_hide(self):
        self.hide()
        # If the shield is the parent, hide it so sidebar becomes clickable
        if self.parent(): self.parent().hide()
        try: self.anim.finished.disconnect(self._finalize_hide)
        except: pass

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.is_dragging = True
            self.drag_offset = event.position().toPoint()
            event.accept()

    def mouseMoveEvent(self, event):
        if self.is_dragging:
            new_pos = self.mapToParent(event.position().toPoint()) - self.drag_offset
            parent_rect = self.parent().rect()
            max_x = parent_rect.width() - self.width()
            max_y = parent_rect.height() - self.height()
            clamped_x = max(0, min(new_pos.x(), max_x))
            clamped_y = max(0, min(new_pos.y(), max_y))
            self.move(clamped_x, clamped_y)
            event.accept()

    def mouseReleaseEvent(self, event):
        self.is_dragging = False
        event.accept()

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