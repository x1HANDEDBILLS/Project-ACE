import numpy as np
from PySide6.QtWidgets import QWidget, QHBoxLayout, QVBoxLayout, QLabel
from PySide6.QtGui import QPainter, QPen, QPainterPath, QColor, QBrush, QImage, QPixmap, QGuiApplication
from PySide6.QtCore import Qt, QTimer, QPropertyAnimation, QEasingCurve, QRect, QPoint, Signal

import theme
from effects.animations import TacticalGlitchEffect, GLOBAL_TELEMETRY
from widgets.signal import SignalIcon
from widgets.button_mini import MiniTacticalButton

GLITCH_MARGIN = 40

class TacticalPopout(QWidget):
    # Signal to tell DashboardPanel we closed
    closed_manually = Signal()

    def __init__(self, parent):
        super().__init__(parent)
        self.setWindowFlags(Qt.SubWindow | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        
        # Base size calculation
        self.base_w, self.base_h = 520, 440
        self.total_w = self.base_w + (GLITCH_MARGIN * 2)
        self.total_h = self.base_h + (GLITCH_MARGIN * 2)
        self.setFixedSize(self.total_w, self.total_h)
        
        self._bg_pixmap = None
        self._is_closing = False
        self.is_dragging = False
        self.drag_offset = QPoint()

        # --- UI LAYOUT ---
        
        # 1. Title
        self.title = QLabel("// SYSTEM_READY", self)
        
        # 2. Signal Info
        self.sig_container = QWidget(self)
        self.sig_layout = QHBoxLayout(self.sig_container)
        self.sig_layout.setContentsMargins(0, 0, 0, 0)
        self.sig_layout.setSpacing(5)
        
        self.sig_icon = SignalIcon()
        self.sig_label = QLabel("SIGNAL: 0%")
        
        self.sig_layout.addWidget(self.sig_icon)
        self.sig_layout.addWidget(self.sig_label)

        # 3. Dynamic Content Socket
        self.content_area = QWidget(self)
        self.content_layout = QVBoxLayout(self.content_area)
        self.content_layout.setContentsMargins(GLITCH_MARGIN + 20, 50, GLITCH_MARGIN + 20, 70)

        # 4. Close Button
        self.close_btn = MiniTacticalButton("Close", self, small=True)
        self.close_btn.setFixedSize(80, 30)
        self.close_btn.clicked.connect(self.hide_hud)

        # --- EFFECTS & ANIMATION ---
        self.fx = TacticalGlitchEffect()
        self.setGraphicsEffect(self.fx)
        self.glitch_timer = QTimer(self)
        self.glitch_timer.timeout.connect(self._update_cycle)
        
        self.anim = QPropertyAnimation(self, b"geometry")
        self.anim.finished.connect(self._finalize_anim)
        
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)

        # Initial style application
        self.refresh_theme()

    def refresh_theme(self):
        """
        SMART REFRESH: Triggered by theme.py to update colors and 
        regenerate the background gradient.
        """
        h_hex = theme.ACTIVE.get('hex', "#FF1E1E")
        m = GLITCH_MARGIN
        w, h_inner = self.width() - (m*2), self.height() - (m*2)

        # 1. Regenerate Background Cache
        self._bg_pixmap = theme.get_numpy_gradient(w, h_inner)

        # 2. Re-apply Stylesheets
        self.title.setStyleSheet(f"color: {h_hex}; font-family: Consolas; font-weight: bold; font-size: 11px;")
        self.sig_label.setStyleSheet(f"color: {h_hex}; font-family: Consolas; font-size: 11px; font-weight: bold;")

        # 3. Propagate to children
        if hasattr(self.sig_icon, 'refresh_theme'):
            self.sig_icon.refresh_theme()
            
        # Recursive update for any content (like ColorPicker) inside the layout
        for i in range(self.content_layout.count()):
            widget = self.content_layout.itemAt(i).widget()
            if widget and hasattr(widget, 'refresh_theme'):
                widget.refresh_theme()

        self.update()

    def set_content(self, widget, title_text="// SYSTEM_CONFIG"):
        """Plug a widget into the popout and update the header"""
        self.title.setText(title_text)
        self.title.adjustSize()
        
        for i in reversed(range(self.content_layout.count())): 
            w = self.content_layout.itemAt(i).widget()
            if w: w.setParent(None)
        
        self.content_layout.addWidget(widget)

    def show_hud(self):
        """Centered on display with expansion animation"""
        self._is_closing = False
        
        # Failsafe: Ensure background exists before animating
        if self._bg_pixmap is None:
            self.refresh_theme()
        
        screen_geo = QGuiApplication.primaryScreen().geometry()
        global_x = (screen_geo.width() - self.width()) // 2
        global_y = (screen_geo.height() - self.height()) // 2
        
        if self.parent():
            local_pos = self.parent().mapFromGlobal(QPoint(global_x, global_y))
            tx, ty = local_pos.x(), local_pos.y()
        else:
            tx, ty = global_x, global_y

        full_geom = QRect(tx, ty, self.width(), self.height())
        start_geom = QRect(tx, ty + (self.height() // 2), self.width(), 4)
        
        self.setGeometry(start_geom)
        self.show()
        self.raise_()

        self.anim.stop()
        self.anim.setDuration(400)
        self.anim.setEasingCurve(QEasingCurve.OutExpo)
        self.anim.setStartValue(start_geom)
        self.anim.setEndValue(full_geom)
        
        self.fx.intensity = 20
        self.glitch_timer.start(50)
        self.anim.start()

    def hide_hud(self):
        if self._is_closing: return
        self._is_closing = True
        self.fx.intensity = 15
        self.glitch_timer.start(50)
        
        curr = self.geometry()
        end_geom = QRect(curr.x(), curr.y() + (curr.height() // 2), curr.width(), 4)
        
        self.anim.stop()
        self.anim.setDuration(250)
        self.anim.setEasingCurve(QEasingCurve.InBack)
        self.anim.setStartValue(curr)
        self.anim.setEndValue(end_geom)
        self.anim.start()
        
        self.closed_manually.emit()

    def _finalize_anim(self):
        if self._is_closing:
            self.hide()
            if self.parent(): self.parent().hide()
            self._is_closing = False

    def _update_cycle(self):
        if self.fx.intensity > 0:
            self.fx.intensity -= 1
            self.update()
        else:
            self.glitch_timer.stop()

    def _sync_telemetry(self, msg, val, dropping):
        if self._is_closing: return
        self.sig_label.setText(f"SIGNAL: {int(val)}%")
        self.sig_icon.val = int(val)
        self.sig_container.adjustSize()
        self.sig_container.move(self.width() - self.sig_container.width() - GLITCH_MARGIN - 20, GLITCH_MARGIN + 15)

    def paintEvent(self, event):
        p = QPainter(self)
        if not p.isActive(): return
        p.setRenderHint(QPainter.Antialiasing)
        
        m = GLITCH_MARGIN
        w, h = self.width() - (m*2), self.height() - (m*2)
        e = 25.0
        
        # Create tactical notched path
        path = QPainterPath()
        path.moveTo(m+e, m); path.lineTo(m+w-e, m); path.lineTo(m+w, m+e)
        path.lineTo(m+w, m+h-e); path.lineTo(m+w-e, m+h); path.lineTo(m+e, m+h)
        path.lineTo(m, m+h-e); path.lineTo(m, m+e); path.closeSubpath()
        
        p.fillPath(path, QBrush(Qt.black))

        # Failsafe: if pixmap is missing during paint, create it
        if self._bg_pixmap is None:
            self._bg_pixmap = theme.get_numpy_gradient(w, h)
        
        if self._bg_pixmap:
            p.setClipPath(path)
            p.drawPixmap(m, m, self._bg_pixmap)
        
        # Border
        p.setClipping(False)
        p.setPen(QPen(QColor(theme.ACTIVE.get("hex", "#FF1E1E")), 2.0))
        p.drawPath(path)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self._bg_pixmap = None 
        self.title.move(GLITCH_MARGIN + 25, GLITCH_MARGIN + 15)
        self.content_area.setGeometry(self.rect())
        self.close_btn.move(self.width() - self.close_btn.width() - GLITCH_MARGIN - 20, 
                           self.height() - self.close_btn.height() - GLITCH_MARGIN - 20)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            if self.close_btn.geometry().contains(event.position().toPoint()): return
            pos = event.position().toPoint()
            # Disable drag if clicking in content center
            if pos.y() > GLITCH_MARGIN + 50 and pos.y() < self.height() - GLITCH_MARGIN - 50:
                 return
            self.is_dragging = True
            self.drag_offset = event.position().toPoint()
            event.accept()

    def mouseMoveEvent(self, event):
        if self.is_dragging:
            delta = event.position().toPoint() - self.drag_offset
            new_pos = self.pos() + delta
            if self.parent():
                pr = self.parent().rect()
                nx = max(0, min(new_pos.x(), pr.width() - self.width()))
                ny = max(0, min(new_pos.y(), pr.height() - self.height()))
                self.move(nx, ny)
            else: self.move(new_pos)

    def mouseReleaseEvent(self, event):
        self.is_dragging = False