import numpy as np
from PySide6.QtWidgets import QWidget, QHBoxLayout, QLabel
from PySide6.QtGui import QPainter, QPen, QPainterPath, QColor, QBrush, QImage, QPixmap
from PySide6.QtCore import Qt, QTimer, QPropertyAnimation, QEasingCurve, QRect, QPoint, Signal

import theme
from effects.animations import TacticalGlitchEffect, GLOBAL_TELEMETRY
from widgets.signal import SignalIcon
from widgets.button_mini import MiniTacticalButton

GLITCH_MARGIN = 40

class TacticalPopout(QWidget):
    # Signal to tell DashboardPanel we closed so it can reset its state
    closed_manually = Signal()

    def __init__(self, parent):
        super().__init__(parent)
        self.setWindowFlags(Qt.SubWindow | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        
        self.base_w, self.base_h = 500, 320
        # The full size including glitch margins
        self.total_w = self.base_w + (GLITCH_MARGIN * 2)
        self.total_h = self.base_h + (GLITCH_MARGIN * 2)
        self.setFixedSize(self.total_w, self.total_h)
        
        self._bg_pixmap = None
        self._is_closing = False
        self.is_dragging = False
        self.drag_offset = QPoint()

        # UI Components
        self.title = QLabel("// SYSTEM_READY", self)
        self.title.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: Consolas; font-weight: bold; font-size: 11px;")
        
        self.sig_container = QWidget(self)
        sig_layout = QHBoxLayout(self.sig_container)
        sig_layout.setContentsMargins(0, 0, 0, 0)
        sig_layout.setSpacing(5)
        
        self.sig_icon = SignalIcon()
        self.sig_label = QLabel("SIGNAL: 0%")
        self.sig_label.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: Consolas; font-size: 11px; font-weight: bold;")
        
        sig_layout.addWidget(self.sig_icon)
        sig_layout.addWidget(self.sig_label)

        self.close_btn = MiniTacticalButton("Close", self, small=True)
        self.close_btn.setFixedSize(80, 30)
        self.close_btn.clicked.connect(self.hide_hud)

        self.fx = TacticalGlitchEffect()
        self.setGraphicsEffect(self.fx)
        self.glitch_timer = QTimer(self)
        self.glitch_timer.timeout.connect(self._update_cycle)
        
        self.anim = QPropertyAnimation(self, b"geometry")
        # Connect once to handle the cleanup after closing animation
        self.anim.finished.connect(self._finalize_anim)
        
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)

    def show_hud(self):
        """Centered on the physical display using Global-to-Local mapping"""
        self._is_closing = False
        self._bg_pixmap = None 
        
        # 1. Get the geometry of the actual screen (the Pi display)
        # Using primaryScreen() ensures we get the hardware resolution
        from PySide6.QtGui import QGuiApplication
        screen_geo = QGuiApplication.primaryScreen().geometry()
        
        # 2. Calculate where the top-left should be in 'Global' space
        global_x = (screen_geo.width() - self.width()) // 2
        global_y = (screen_geo.height() - self.height()) // 2
        global_target_point = QPoint(global_x, global_y)
        
        # 3. Map that Global point to our Parent's local coordinates
        if self.parent():
            # This is the 'Magic' line that fixes centering issues
            local_point = self.parent().mapFromGlobal(global_target_point)
            target_x, target_y = local_point.x(), local_point.y()
        else:
            target_x, target_y = global_x, global_y

        full_geom = QRect(target_x, target_y, self.width(), self.height())
        
        # 4. Define the 'Collapsed' start state (horizontal line)
        start_geom = QRect(target_x, target_y + (self.height() // 2), self.width(), 4)
        
        # Set initial state
        self.setGeometry(start_geom)
        self.show()
        self.raise_()

        # 5. Run expansion animation
        self.anim.stop()
        self.anim.setDuration(350)
        self.anim.setEasingCurve(QEasingCurve.OutCubic)
        self.anim.setStartValue(start_geom)
        self.anim.setEndValue(full_geom)
        
        self.fx.intensity = 18
        self.glitch_timer.start(50)
        self.anim.start()

    def hide_hud(self):
        """Closing animation: Collapse to center line"""
        if self._is_closing: return
        self._is_closing = True
        
        self.fx.intensity = 15
        self.glitch_timer.start(50)
        
        curr = self.geometry()
        # Collapse to a horizontal line
        end_geom = QRect(curr.x(), curr.y() + (curr.height() // 2), curr.width(), 4)
        
        self.anim.stop()
        self.anim.setDuration(250)
        self.anim.setEasingCurve(QEasingCurve.InBack)
        self.anim.setStartValue(curr)
        self.anim.setEndValue(end_geom)
        self.anim.start()
        
        # Notify Dashboard state
        self.closed_manually.emit()

    def _finalize_anim(self):
        """Only hides the widget if we were in the middle of closing"""
        if self._is_closing:
            self.hide()
            if self.parent():
                self.parent().hide()
            self._is_closing = False

    def _update_cycle(self):
        if self.fx.intensity > 0:
            self.fx.intensity -= 1
            self.update()
        else:
            self.glitch_timer.stop()

    def _sync_telemetry(self, msg, val, dropping):
        if self._is_closing: return
        self.title.setText(msg)
        self.sig_label.setText(f"SIGNAL: {int(val)}%")
        self.sig_icon.val = int(val)
        self.title.adjustSize()
        self.sig_container.adjustSize()
        self.sig_container.move(self.width() - self.sig_container.width() - 25, 15)

    def paintEvent(self, event):
        p = QPainter(self)
        if not p.isActive(): return
        p.setRenderHint(QPainter.Antialiasing)
        
        w, h = self.width(), self.height()
        e = 25.0 # Notch size
        
        path = QPainterPath()
        path.moveTo(e, 0); path.lineTo(w-e, 0); path.lineTo(w, e); path.lineTo(w, h-e)
        path.lineTo(w-e, h); path.lineTo(e, h); path.lineTo(0, h-e); path.lineTo(0, e); path.closeSubpath()
        
        p.fillPath(path, QBrush(Qt.black))

        if self._bg_pixmap is None:
            self._bg_pixmap = theme.get_numpy_gradient(w, h)

        if self._bg_pixmap:
            p.setClipPath(path)
            p.drawPixmap(0, 0, self._bg_pixmap)
        
        p.setClipping(False)
        p.setPen(QPen(QColor(theme.ACTIVE["hex"]), 2.5))
        p.drawPath(path)
        p.end()

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self._bg_pixmap = None 
        self.title.move(25, 15)
        self.close_btn.move(self.width() - self.close_btn.width() - 15, 
                           self.height() - self.close_btn.height() - 15)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            if self.close_btn.geometry().contains(event.position().toPoint()): return
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
            else:
                self.move(new_pos)
            event.accept()

    def mouseReleaseEvent(self, event):
        self.is_dragging = False