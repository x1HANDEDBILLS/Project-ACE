import sys
import numpy as np
from PySide6.QtWidgets import QFrame, QVBoxLayout, QHBoxLayout, QLabel, QStackedWidget, QWidget
from PySide6.QtCore import Qt
from PySide6.QtGui import QImage, QPixmap, QPainter, QPainterPath
import theme

# Shared Global Telemetry for synced animations
from effects.animations import GLOBAL_TELEMETRY
from widgets.frame import CustomFrame
from widgets.button import CustomButton
from widgets.signal import SignalIcon

# Import all 5 modular Settings pages
from dash.settings_page1 import SettingsPage1
from dash.settings_page2 import SettingsPage2
from dash.settings_page3 import SettingsPage3
from dash.settings_page4 import SettingsPage4
from dash.settings_page5 import SettingsPage5

class SettingsPanel(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # 1. WINDOW ATTRIBUTES
        self.setAttribute(Qt.WA_StyledBackground)
        self.setStyleSheet("background: transparent; border: none;")
        self.setObjectName("MainSettings")
        
        # 2. TELEMETRY CONNECTION
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)
        
        # 3. BACKGROUND INITIALIZATION
        self.set_gradient_bg(834, 600)
        
        # 4. LAYOUT STRUCTURE
        self.outer_layout = QVBoxLayout(self)
        self.outer_layout.setContentsMargins(10, 10, 10, 10) 
        
        # The HUD Box with 45-degree notches
        self.content_frame = CustomFrame(self, edge_size=25.0, border_width=2.0, bg_alpha=0)
        self.inner_layout = self.content_frame.container
        self.inner_layout.setContentsMargins(0, 0, 0, 0)
        self.inner_layout.setSpacing(0)
        
        # 5. HEADER SECTION
        self.header_widget = QWidget()
        self.header_layout = QHBoxLayout(self.header_widget)
        self.header_layout.setContentsMargins(20, 10, 20, 0) 
        self.header_layout.setSpacing(0)
        
        # LEFT: Status
        self.status_container = QWidget()
        self.status_vbox = QVBoxLayout(self.status_container)
        self.status_vbox.setContentsMargins(0, 0, 0, 0)
        self.status_label = QLabel("// INITIALIZING...")
        self.status_vbox.addSpacing(5)
        self.status_vbox.addWidget(self.status_label)
        self.status_vbox.addStretch()
        
        # CENTER: Title
        self.header = QLabel("SETTINGS")
        self.header.setAlignment(Qt.AlignCenter)
        
        # RIGHT: Signal
        self.sig_container = QWidget()
        self.sig_layout = QHBoxLayout(self.sig_container)
        self.sig_layout.setContentsMargins(0, 0, 0, 0)
        self.sig_icon = SignalIcon()
        self.sig_icon.setFixedSize(22, 22)
        self.sig_pct = QLabel("SIGNAL: 0%")
        self.sig_layout.addWidget(self.sig_icon)
        self.sig_layout.addWidget(self.sig_pct)

        self.header_layout.addWidget(self.status_container, 1, Qt.AlignLeft | Qt.AlignTop)
        self.header_layout.addWidget(self.header, 2, Qt.AlignHCenter | Qt.AlignTop)
        self.header_layout.addWidget(self.sig_container, 1, Qt.AlignRight | Qt.AlignTop)
        self.inner_layout.addWidget(self.header_widget)

        # 6. PAGE NAVIGATION
        self.pages = QStackedWidget()
        self.pages.setStyleSheet("background: transparent; border: none;")
        self.page_titles = ["SETTINGS", "USER INTERFACE", "NETWORK", "SECURITY", "SYSTEM"]
        
        # Initialize instances
        self.p1 = SettingsPage1(self)
        self.p2 = SettingsPage2(self)
        self.p3 = SettingsPage3(self)
        self.p4 = SettingsPage4(self)
        self.p5 = SettingsPage5(self)
        
        # Add to stack
        self.pages.addWidget(self.p1)
        self.pages.addWidget(self.p2)
        self.pages.addWidget(self.p3)
        self.pages.addWidget(self.p4)
        self.pages.addWidget(self.p5)
        
        self.inner_layout.addWidget(self.pages, 1)

        # 7. FOOTER SECTION
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

        self.refresh_theme()

    def refresh_theme(self):
        h = theme.ACTIVE['hex']
        style = f"color: {h}; font-family: 'Consolas'; font-weight: bold; background: none; border: none;"
        
        self.status_label.setStyleSheet(style + "font-size: 14px;")
        self.header.setStyleSheet(style + "font-size: 32px;")
        self.sig_pct.setStyleSheet(style + "font-size: 14px;")
        self.page_label.setStyleSheet(style + "font-size: 22px;")
        
        # Refresh all child pages
        for i in range(self.pages.count()):
            page = self.pages.widget(i)
            if hasattr(page, 'refresh_theme'):
                page.refresh_theme()

        self.prev_btn.update()
        self.next_btn.update()
        self.content_frame.update()
        self.update()

    def _sync_telemetry(self, msg, val, dropping):
        self.status_label.setText(msg)
        self.sig_pct.setText(f"SIGNAL: {int(val)}%")
        self.sig_icon.val = int(val)
        self.sig_icon.update()

    def update_state(self, proc):
        curr = self.pages.currentWidget()
        if hasattr(curr, 'update_state'):
            curr.update_state(proc)

    def set_gradient_bg(self, width, height):
        gradient = np.linspace(8, 22, height, dtype=np.uint8)
        pattern = np.repeat(gradient[:, np.newaxis], width, axis=1)
        image_array = np.stack((pattern, pattern, pattern), axis=-1)
        qimg = QImage(image_array.data, width, height, width * 3, QImage.Format_RGB888)
        self._bg_pixmap = QPixmap.fromImage(qimg)

    def paintEvent(self, event):
        with QPainter(self) as painter:
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
        idx = self.pages.currentIndex()
        self.header.setText(self.page_titles[idx])
        self.page_label.setText(f"{idx + 1} / 5")

    def next_page(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() + 1) % 5)
        self.update_page_ui()

    def prev_page(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() - 1) % 5)
        self.update_page_ui()