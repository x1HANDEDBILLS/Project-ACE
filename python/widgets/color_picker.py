from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QGridLayout, QDialog
from PySide6.QtGui import QPainter, QColor, QLinearGradient
from PySide6.QtCore import Qt
import theme
from gui.save import save_visual_settings

class ColorSquare(QWidget):
    def __init__(self):
        super().__init__()
        self.setMinimumSize(250, 250)
        self.hue = 0 
        self.sat = 255
        self.val = 255

    def paintEvent(self, event):
        p = QPainter(self)
        # 1. Base Gradient (White to Hue)
        h_grad = QLinearGradient(0, 0, self.width(), 0)
        h_grad.setColorAt(0, Qt.white)
        # Clamping to 359 to prevent the QColor HSV out of range error
        safe_hue = max(0, min(359, int(self.hue)))
        h_grad.setColorAt(1, QColor.fromHsv(safe_hue, 255, 255))
        p.fillRect(self.rect(), h_grad)
        
        # 2. Overlay Gradient (Transparent to Black)
        v_grad = QLinearGradient(0, 0, 0, self.height())
        v_grad.setColorAt(0, QColor(0, 0, 0, 0))
        v_grad.setColorAt(1, Qt.black)
        p.fillRect(self.rect(), v_grad)

    def mousePressEvent(self, event): self.process_input(event)
    def mouseMoveEvent(self, event): 
        if event.buttons() & Qt.LeftButton: self.process_input(event)

    def process_input(self, event):
        pos = event.position()
        x_ratio = max(0.0, min(pos.x() / (self.width() or 1), 1.0))
        y_ratio = max(0.0, min(pos.y() / (self.height() or 1), 1.0))
        
        # Internal state for direct saving
        self.sat = int(x_ratio * 255)
        self.val = int(255 - (y_ratio * 255))
        
        try:
            new_color = QColor.fromHsv(int(self.hue), self.sat, self.val)
            if new_color.isValid():
                # Theme is only used for live previewing
                theme.set_active_theme((new_color.red(), new_color.green(), new_color.blue()))
                self.trigger_refresh()
        except: pass

    def trigger_refresh(self):
        main_win = self.window()
        if main_win.parent() and hasattr(main_win.parent(), 'refresh_dashboard'):
            main_win.parent().refresh_dashboard()

class HueSlider(QWidget):
    def __init__(self, square):
        super().__init__()
        self.square = square
        self.setFixedWidth(30)
        self.setMinimumHeight(250)

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
        y_ratio = max(0.0, min(pos.y() / (self.height() or 1), 0.999))
        self.square.hue = int(y_ratio * 360)
        self.square.update()

class ColorEngineBox(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(12)
        
        selection_layout = QHBoxLayout()
        self.square = ColorSquare()
        self.slider = HueSlider(self.square)
        selection_layout.addWidget(self.square)
        selection_layout.addWidget(self.slider)
        layout.addLayout(selection_layout)

        grid = QGridLayout()
        presets = ["#FF0000", "#FF8C00", "#FFD700", "#00FF00", "#00FFFF", 
                   "#1E90FF", "#8A2BE2", "#FF00FF", "#FFFFFF", "#808080"]
        for i, color_hex in enumerate(presets):
            btn = QPushButton()
            btn.setFixedSize(50, 30)
            btn.setStyleSheet(f"background: {color_hex}; border: 1px solid #333;")
            btn.clicked.connect(lambda _, h=color_hex: self.apply_preset(h))
            grid.addWidget(btn, i//5, i%5)
        layout.addLayout(grid)

        # --- THE DIRECT SAVE BUTTON ---
        self.save_btn = QPushButton("SAVE SETTINGS")
        self.save_btn.setFixedHeight(45)
        self.save_btn.setStyleSheet("""
            QPushButton {
                background-color: #111;
                color: #EEE;
                border: 1px solid #444;
                font-weight: bold;
                border-radius: 4px;
            }
            QPushButton:pressed {
                background-color: #333;
                border: 1px solid #666;
            }
        """)
        self.save_btn.clicked.connect(self.manual_save)
        layout.addWidget(self.save_btn)

    def manual_save(self):
        hue = max(0, min(359, int(self.square.hue)))
        sat = self.square.sat
        val = self.square.val
        direct_hex = QColor.fromHsv(hue, sat, val).name()
        
        save_visual_settings(direct_hex)
        
        # --- THE FIX: Tell the dashboard to update NOW ---
        main_win = self.window()
        # Find the DashboardContent widget and tell it to refresh
        if hasattr(main_win, 'dashboard'):
            main_win.dashboard.refresh_dashboard()
        # Also tell the main window itself to refresh its global QSS
        main_win.setStyleSheet(theme.get_qss())

    def apply_preset(self, color_hex):
        c = QColor(color_hex)
        theme.set_active_theme((c.red(), c.green(), c.blue()))
        h, s, v, a = c.getHsv()
        self.square.hue = max(0, h)
        self.square.sat = s
        self.square.val = v
        self.square.update()
        
        # Also auto-save the preset choice
        save_visual_settings(color_hex)
        
        main_win = self.window()
        if main_win.parent() and hasattr(main_win.parent(), 'refresh_dashboard'):
            main_win.parent().refresh_dashboard()

class ThemePopup(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        self.setStyleSheet("background: #0A0A0A; border: 1px solid #444; padding: 15px;")
        
        layout = QVBoxLayout(self)
        self.engine = ColorEngineBox()
        layout.addWidget(self.engine)