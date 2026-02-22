import sys
import random
import numpy as np
from PySide6.QtWidgets import QFrame, QVBoxLayout, QHBoxLayout, QLabel, QStackedWidget, QWidget
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QImage, QPixmap, QPainter, QPainterPath
import theme

# Shared Global Telemetry for synced animations
from effects.animations import GLOBAL_TELEMETRY
from widgets.frame import CustomFrame
from widgets.button import CustomButton
from widgets.signal import SignalIcon
from dash.settings_page1 import SettingsPage1

class SettingsPanel(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.setAttribute(Qt.WA_StyledBackground)
        self.setStyleSheet("background: transparent; border: none;")
        self.setObjectName("MainSettings")
        
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)
        self.set_gradient_bg(834, 600)
        
        self.outer_layout = QVBoxLayout(self)
        self.outer_layout.setContentsMargins(10, 10, 10, 10) 
        
        self.content_frame = CustomFrame(self, edge_size=25.0, border_width=2.0, bg_alpha=0)
        self.inner_layout = self.content_frame.container
        self.inner_layout.setContentsMargins(0, 0, 0, 0)
        self.inner_layout.setSpacing(0)
        
        # --- HEADER SECTION ---
        self.header_widget = QWidget()
        self.header_layout = QHBoxLayout(self.header_widget)
        self.header_layout.setContentsMargins(20, 10, 20, 0) 
        
        self.status_container = QWidget()
        self.status_vbox = QVBoxLayout(self.status_container)
        self.status_vbox.setContentsMargins(0, 0, 0, 0)
        self.status_vbox.addSpacing(5) 
        
        self.status_label = QLabel("// INITIALIZING...")
        self.status_label.setAttribute(Qt.WA_TranslucentBackground)
        
        self.header = QLabel("SETTINGS")
        self.header.setAlignment(Qt.AlignCenter)
        self.header.setAttribute(Qt.WA_TranslucentBackground)
        
        self.sig_container = QWidget()
        self.sig_layout = QHBoxLayout(self.sig_container)
        self.sig_layout.setContentsMargins(0, 0, 0, 0)
        
        self.sig_icon = SignalIcon()
        self.sig_icon.setFixedSize(22, 22)
        self.sig_icon.val = 95
        
        self.sig_pct = QLabel("SIGNAL: 95%")
        
        self.sig_layout.addWidget(self.sig_icon)
        self.sig_layout.addWidget(self.sig_pct)

        self.header_layout.addWidget(self.status_container, 1, Qt.AlignLeft | Qt.AlignTop)
        self.header_layout.addWidget(self.header, 2, Qt.AlignHCenter | Qt.AlignTop)
        self.header_layout.addWidget(self.sig_container, 1, Qt.AlignRight | Qt.AlignTop)
        self.inner_layout.addWidget(self.header_widget)

        # --- PAGES ---
        self.pages = QStackedWidget()
        self.page_titles = ["SETTINGS", "USER INTERFACE", "NETWORK", "SECURITY", "SYSTEM"]
        
        self.p1 = SettingsPage1(self)
        self.pages.addWidget(self.p1)
        
        self.dummy_labels = [] # Keep track of labels in dummy pages to update color
        for i in range(1, 5):
            page = QWidget()
            l = QVBoxLayout(page)
            msg = QLabel(f"TERMINAL {self.page_titles[i]}\n> STATUS: ONLINE")
            msg.setAlignment(Qt.AlignCenter)
            l.addWidget(msg)
            self.pages.addWidget(page)
            self.dummy_labels.append(msg)
            
        self.inner_layout.addWidget(self.pages, 1)

        # --- FOOTER ---
        self.footer_container = QWidget()
        self.footer_layout = QHBoxLayout(self.footer_container)
        self.footer_layout.setContentsMargins(0, 0, 0, 10) 
        self.footer_layout.setSpacing(20)
        self.footer_layout.setAlignment(Qt.AlignCenter)
        
        self.prev_btn = CustomButton("<", self, small=True)
        self.prev_btn.setFixedSize(80, 55)
        self.prev_btn.font_size = 24
        self.prev_btn.clicked.connect(self.prev_page)
        
        self.page_label = QLabel("1 / 5")
        self.page_label.setFixedWidth(100)
        self.page_label.setAlignment(Qt.AlignCenter)
        
        self.next_btn = CustomButton(">", self, small=True)
        self.next_btn.setFixedSize(80, 55)
        self.next_btn.font_size = 24
        self.next_btn.clicked.connect(self.next_page)
        
        self.footer_layout.addWidget(self.prev_btn)
        self.footer_layout.addWidget(self.page_label)
        self.footer_layout.addWidget(self.next_btn)
        self.inner_layout.addWidget(self.footer_container)
        self.outer_layout.addWidget(self.content_frame)

        # APPLY THE INITIAL THEME COLORS
        self.refresh_theme()

    def refresh_theme(self):
        """SMART REFRESH: Re-applies active hex color to all UI elements"""
        h = theme.ACTIVE['hex']
        
        # 1. Update Header Labels
        self.status_label.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 14px; font-weight: bold; background: none; border: none;")
        self.header.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 32px; font-weight: bold; background: none; border: none;")
        self.sig_pct.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 14px; font-weight: bold; background: none; border: none;")
        
        # 2. Update Footer & Pagination
        self.page_label.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 22px; font-weight: bold; background: none;")
        self.prev_btn.update()
        self.next_btn.update()
        
        # 3. Update Dummy Page Labels
        for lbl in self.dummy_labels:
            lbl.setStyleSheet(f"color: {h}; font-family: 'Consolas'; font-size: 20px; background: none;")
        
        # 4. Refresh Nested Widgets (Page 1 and Signal Icon)
        if hasattr(self.p1, 'refresh_theme'): self.p1.refresh_theme()
        if hasattr(self.sig_icon, 'refresh_theme'): self.sig_icon.refresh_theme()
        
        # 5. Tell the notched frame to repaint its border
        self.content_frame.update()
        self.update()

    def _sync_telemetry(self, msg, val, dropping):
        self.status_label.setText(msg)
        self.sig_pct.setText(f"SIGNAL: {int(val)}%")
        self.sig_icon.val = int(val)
        self.sig_icon.update()

    def set_gradient_bg(self, width, height):
        gradient = np.linspace(8, 22, height, dtype=np.uint8)
        pattern = np.repeat(gradient[:, np.newaxis], width, axis=1)
        image_array = np.stack((pattern, pattern, pattern), axis=-1)
        qimg = QImage(image_array.data, width, height, width * 3, QImage.Format_RGB888)
        self._bg_pixmap = QPixmap.fromImage(qimg)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        rect = self.content_frame.geometry()
        edge = 25.0 
        path = QPainterPath()
        path.moveTo(rect.left() + edge, rect.top())
        path.lineTo(rect.right() - edge, rect.top())
        path.lineTo(rect.right(), rect.top() + edge)
        path.lineTo(rect.right(), rect.bottom() - edge)
        path.lineTo(rect.right() - edge, rect.bottom())
        path.lineTo(rect.left() + edge, rect.bottom())
        path.lineTo(rect.left(), rect.bottom() - edge)
        path.lineTo(rect.left(), rect.top() + edge)
        path.closeSubpath()
        painter.setClipPath(path)
        painter.drawPixmap(self.rect(), self._bg_pixmap)

    def update_page_ui(self):
        index = self.pages.currentIndex()
        self.header.setText(self.page_titles[index])
        self.page_label.setText(f"{index + 1} / 5")

    def next_page(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() + 1) % 5)
        self.update_page_ui()

    def prev_page(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() - 1) % 5)
        self.update_page_ui()

    def update_state(self, proc):
        pass