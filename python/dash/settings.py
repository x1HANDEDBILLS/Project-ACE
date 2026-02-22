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
        
        # 1. WINDOW ATTRIBUTES
        self.setAttribute(Qt.WA_StyledBackground)
        self.setStyleSheet("background: transparent; border: none;")
        self.setObjectName("MainSettings")
        
        # 2. TELEMETRY CONNECTION
        # Connect to the global broadcaster so this and the Popout pulse in sync
        GLOBAL_TELEMETRY.updated.connect(self._sync_telemetry)
        
        # 3. BACKGROUND INITIALIZATION
        # Generate the pixel data for the tactical vertical gradient (834x600)
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
        
        # LEFT SIDE: Status (with the 5px vertical nudge)
        self.status_container = QWidget()
        self.status_vbox = QVBoxLayout(self.status_container)
        self.status_vbox.setContentsMargins(0, 0, 0, 0)
        self.status_vbox.addSpacing(5) # The specific 5px request
        
        self.status_label = QLabel("// INITIALIZING...")
        self.status_label.setAttribute(Qt.WA_TranslucentBackground)
        self.status_label.setStyleSheet(f"""
            color: {theme.ACTIVE['hex']}; 
            font-family: 'Consolas'; 
            font-size: 14px; 
            font-weight: bold; 
            background: none; 
            border: none;
        """)
        self.status_vbox.addWidget(self.status_label)
        self.status_vbox.addStretch()
        
        # CENTER: Main Title
        self.header = QLabel("SETTINGS")
        self.header.setAlignment(Qt.AlignCenter)
        self.header.setAttribute(Qt.WA_TranslucentBackground)
        self.header.setStyleSheet(f"""
            color: {theme.ACTIVE['hex']}; 
            font-family: 'Consolas'; 
            font-size: 32px; 
            font-weight: bold; 
            background: none; 
            border: none;
        """)
        
        # RIGHT SIDE: Signal Status
        self.sig_container = QWidget()
        self.sig_container.setStyleSheet("background: transparent;")
        self.sig_layout = QHBoxLayout(self.sig_container)
        self.sig_layout.setContentsMargins(0, 0, 0, 0)
        
        self.sig_icon = SignalIcon()
        self.sig_icon.setFixedSize(22, 22)
        self.sig_icon.val = 95
        
        self.sig_pct = QLabel("SIGNAL: 95%")
        self.sig_pct.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: 'Consolas'; font-size: 14px; font-weight: bold; background: none; border: none;")
        
        self.sig_layout.addWidget(self.sig_icon)
        self.sig_layout.addWidget(self.sig_pct)

        self.header_layout.addWidget(self.status_container, 1, Qt.AlignLeft | Qt.AlignTop)
        self.header_layout.addWidget(self.header, 2, Qt.AlignHCenter | Qt.AlignTop)
        self.header_layout.addWidget(self.sig_container, 1, Qt.AlignRight | Qt.AlignTop)
        self.inner_layout.addWidget(self.header_widget)

        # 6. PAGE NAVIGATION (Stacked Widget)
        self.pages = QStackedWidget()
        self.pages.setStyleSheet("background: transparent; border: none;")
        self.page_titles = ["SETTINGS", "USER INTERFACE", "NETWORK", "SECURITY", "SYSTEM"]
        
        # Load the custom Page 1 (Theme selection page)
        self.p1 = SettingsPage1(self)
        self.pages.addWidget(self.p1)
        
        # Dummy pages for 2 through 5
        for i in range(1, 5):
            page = QWidget()
            l = QVBoxLayout(page)
            msg = QLabel(f"TERMINAL {self.page_titles[i]}\n> STATUS: ONLINE")
            msg.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: 'Consolas'; font-size: 20px; background: none;")
            msg.setAlignment(Qt.AlignCenter)
            l.addWidget(msg)
            self.pages.addWidget(page)
            
        self.inner_layout.addWidget(self.pages, 1)

        # 7. FOOTER SECTION (Enlarged for tactile use)
        self.footer_container = QWidget()
        self.footer_layout = QHBoxLayout(self.footer_container)
        self.footer_layout.setContentsMargins(0, 0, 0, 10) 
        self.footer_layout.setSpacing(20)
        self.footer_layout.setAlignment(Qt.AlignCenter)
        
        # Bigger buttons: 80x55
        self.prev_btn = CustomButton("<", self, small=True)
        self.prev_btn.setFixedSize(80, 55)
        self.prev_btn.font_size = 24
        self.prev_btn.clicked.connect(self.prev_page)
        
        self.page_label = QLabel("1 / 5")
        self.page_label.setFixedWidth(100)
        self.page_label.setAlignment(Qt.AlignCenter)
        self.page_label.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: 'Consolas'; font-size: 22px; font-weight: bold; background: none;")
        
        self.next_btn = CustomButton(">", self, small=True)
        self.next_btn.setFixedSize(80, 55)
        self.next_btn.font_size = 24
        self.next_btn.clicked.connect(self.next_page)
        
        self.footer_layout.addWidget(self.prev_btn)
        self.footer_layout.addWidget(self.page_label)
        self.footer_layout.addWidget(self.next_btn)
        
        self.inner_layout.addWidget(self.footer_container)
        self.outer_layout.addWidget(self.content_frame)

    # --- LOGIC METHODS ---

    def _sync_telemetry(self, msg, val, dropping):
        """Update UI based on the Global Broadcaster signal"""
        self.status_label.setText(msg)
        self.sig_pct.setText(f"SIGNAL: {int(val)}%")
        self.sig_icon.val = int(val)
        self.sig_icon.update()

    def set_gradient_bg(self, width, height):
        """Builds a NumPy array gradient for the tactical background"""
        gradient = np.linspace(8, 22, height, dtype=np.uint8)
        pattern = np.repeat(gradient[:, np.newaxis], width, axis=1)
        image_array = np.stack((pattern, pattern, pattern), axis=-1)
        qimg = QImage(image_array.data, width, height, width * 3, QImage.Format_RGB888)
        self._bg_pixmap = QPixmap.fromImage(qimg)

    def paintEvent(self, event):
        """Draws the gradient background with clipped 45-degree corners"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Calculate clipping path to match CustomFrame notches
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
        """Updates the Header and Footer text based on current StackedWidget index"""
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
        """Reserved for process/telemetry updates from dashboard_panel.py"""
        pass