import sys
import os

# Add project root to path
current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(current_dir)
if project_root not in sys.path:
    sys.path.insert(0, project_root)

from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QGridLayout
from PySide6.QtGui import QPainter, QColor, QLinearGradient
from PySide6.QtCore import Qt, QTimer
import theme
from saves.color_save import save_visual_settings

class ColorSquare(QWidget):
    def __init__(self):
        super().__init__()
        # BIGGER: Increased from 140 to 180 for better touch/mouse control
        self.setMinimumSize(180, 180)
        self.hue = 0 
        self.sat = 255
        self.val = 255

    def paintEvent(self, event):
        p = QPainter(self)
        h_grad = QLinearGradient(0, 0, self.width(), 0)
        h_grad.setColorAt(0, Qt.white)
        safe_hue = max(0, min(359, int(self.hue)))
        h_grad.setColorAt(1, QColor.fromHsv(safe_hue, 255, 255))
        p.fillRect(self.rect(), h_grad)
        
        v_grad = QLinearGradient(0, 0, 0, self.height())
        v_grad.setColorAt(0, QColor(0, 0, 0, 0))
        v_grad.setColorAt(1, Qt.black)
        p.fillRect(self.rect(), v_grad)

    def mousePressEvent(self, event): self.process_input(event)
    def mouseMoveEvent(self, event): 
        if event.buttons() & Qt.LeftButton: self.process_input(event)

    def process_input(self, event):
        pos = event.position()
        x_ratio = max(0.0, min(pos.x() / self.width(), 1.0))
        y_ratio = max(0.0, min(pos.y() / self.height(), 1.0))
        self.sat = int(x_ratio * 255)
        self.val = int(255 - (y_ratio * 255))
        
        new_color = QColor.fromHsv(int(self.hue), self.sat, self.val)
        if new_color.isValid():
            theme.set_active_theme((new_color.red(), new_color.green(), new_color.blue()))
            self.trigger_refresh()

    def trigger_refresh(self):
        win = self.window()
        if hasattr(win, 'dashboard') and hasattr(win.dashboard, 'refresh_dashboard'):
            win.dashboard.refresh_dashboard()

class HueSlider(QWidget):
    def __init__(self, square):
        super().__init__()
        self.square = square
        # BIGGER: Matches the 180 height
        self.setFixedSize(30, 180)

    def paintEvent(self, event):
        p = QPainter(self)
        grad = QLinearGradient(0, 0, 0, self.height())
        for i in range(361): 
            grad.setColorAt(i/360, QColor.fromHsv(min(i, 359), 255, 255))
        p.fillRect(self.rect(), grad)

    def mousePressEvent(self, event): self.process_hue(event)
    def mouseMoveEvent(self, event):
        if event.buttons() & Qt.LeftButton: self.process_hue(event)

    def process_hue(self, event):
        pos = event.position()
        y_ratio = max(0.0, min(pos.y() / self.height(), 0.999))
        self.square.hue = int(y_ratio * 360)
        self.square.update()
        new_color = QColor.fromHsv(self.square.hue, self.square.sat, self.square.val)
        theme.set_active_theme((new_color.red(), new_color.green(), new_color.blue()))
        self.square.trigger_refresh()

class ColorEngineBox(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setAttribute(Qt.WA_TranslucentBackground)
        # BIGGER: Main container expanded
        self.setMinimumSize(280, 340)
        
        layout = QVBoxLayout(self)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(15)
        layout.setAlignment(Qt.AlignCenter)
        
        selection_layout = QHBoxLayout()
        selection_layout.setSpacing(15)
        self.square = ColorSquare()
        self.slider = HueSlider(self.square)
        selection_layout.addWidget(self.square)
        selection_layout.addWidget(self.slider)
        layout.addLayout(selection_layout)

        grid = QGridLayout()
        grid.setSpacing(8)
        presets = ["#FF0000", "#FF8C00", "#FFD700", "#00FF00", "#00FFFF", 
                   "#1E90FF", "#8A2BE2", "#FF00FF", "#FFFFFF", "#555555"]
        for i, color_hex in enumerate(presets):
            btn = QPushButton()
            btn.setFixedSize(45, 25) # Slightly larger buttons
            btn.setStyleSheet(f"background: {color_hex}; border: 1px solid #444; border-radius: 2px;")
            btn.clicked.connect(lambda _, h=color_hex: self.apply_preset(h))
            grid.addWidget(btn, i//5, i%5)
        layout.addLayout(grid)

        # SAVE BUTTON WITH FEEDBACK LOGIC
        self.save_btn = QPushButton("Save Theme Color")
        self.save_btn.setFixedHeight(40)
        self.update_save_button_style("#444")
        self.save_btn.clicked.connect(self.manual_save)
        layout.addWidget(self.save_btn)

    def update_save_button_style(self, color):
        self.save_btn.setStyleSheet(f"""
            QPushButton {{
                background-color: #000;
                color: {color};
                border: 2px solid {color};
                font-family: 'Consolas';
                font-size: 12px;
                font-weight: bold;
                text-transform: uppercase;
            }}
            QPushButton:pressed {{ background-color: #222; }}
        """)

    def manual_save(self):
        final_color = QColor.fromHsv(int(self.square.hue), self.square.sat, self.square.val)
        hex_code = final_color.name()
        
        # Save to JSON
        save_visual_settings(hex_code)
        
        # VISUAL FEEDBACK: Change button to theme color to show success
        self.update_save_button_style(hex_code)
        self.save_btn.setText("SAVED SUCCESSFUL")
        
        # Reset text after 1.5 seconds
        QTimer.singleShot(1500, lambda: self.save_btn.setText("Save Theme Color"))
        
        self.square.trigger_refresh()

    def apply_preset(self, color_hex):
        c = QColor(color_hex)
        theme.set_active_theme((c.red(), c.green(), c.blue()))
        h, s, v, a = c.getHsv()
        self.square.hue, self.square.sat, self.square.val = max(0, h), s, v
        self.square.update()
        self.square.trigger_refresh()