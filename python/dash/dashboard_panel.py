from PySide6.QtWidgets import QWidget, QFrame, QVBoxLayout
from PySide6.QtCore import Qt, QPropertyAnimation, QEasingCurve, QPoint
from widgets.button import CustomButton 
from dash.panel_buttons import PanelButtons
from dash.settings import SettingsPanel # Import the separate file
import theme

class DashboardPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # 1. MAIN HUD GEAR
        self.settings_btn = CustomButton("⚙", self)
        self.settings_btn.setFixedSize(80, 80)
        self.settings_btn.font_size = 42 
        self.settings_btn.edge_size = 12.0
        self.settings_btn.clicked.connect(self.toggle_sidebar)

        # 2. SHIELD (Blocking clicks on the background)
        self.shield = QFrame(self)
        self.shield.hide()
        self.shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")
        self.shield.mousePressEvent = lambda e: e.accept()

        # 3. SETTINGS GUI (Modularly loaded)
        self.sub_panel = SettingsPanel(self)
        self.sub_panel.hide()

        # 4. SIDEBAR (Fixed 190px)
        self.sidebar_width = 190
        self.sidebar = QFrame(self)
        self.sidebar.setAttribute(Qt.WA_StyledBackground)
        self.sidebar.setStyleSheet(f"QFrame {{ background-color: rgba(5, 5, 5, 180); border-left: 2px solid {theme.ACTIVE['hex']}; }}")
        
        self.sidebar_container = QVBoxLayout(self.sidebar)
        self.sidebar_container.setContentsMargins(0, 0, 0, 0)
        self.button_grid = PanelButtons(self.sidebar)
        self.sidebar_container.addWidget(self.button_grid)
        
        # Connect Grid Buttons
        self.button_grid.back_btn.clicked.connect(self.smart_back)
        self.button_grid.inner_settings_btn.clicked.connect(self.toggle_sub_panel)

        # Animation (Sidebar ONLY)
        self.sidebar_open = False
        self.sub_open = False
        self.anim_side = QPropertyAnimation(self.sidebar, b"pos")
        self.anim_side.setDuration(250)
        self.anim_side.setEasingCurve(QEasingCurve.OutCubic)

    def toggle_sidebar(self):
        w, h = self.width(), self.height()
        if not self.sidebar_open:
            self.shield.setGeometry(0, 0, w, h)
            self.shield.show()
            self.sidebar.show()
            self.shield.raise_() 
            self.sidebar.raise_()
            self.anim_side.setStartValue(QPoint(w, 0))
            self.anim_side.setEndValue(QPoint(w - self.sidebar_width, 0))
            self.sidebar_open = True
            self.anim_side.start()
        else:
            self.smart_back()

    def toggle_sub_panel(self):
        if not self.sub_open:
            self.sub_open = True
            self.update_geometries() # Snap to current screen size
            self.sub_panel.show()
            self.sub_panel.raise_()
            self.sidebar.raise_() # Sidebar border overlays the settings
        else:
            self.sub_open = False
            self.sub_panel.hide()

    def smart_back(self):
        if self.sub_open:
            self.sub_open = False
            self.sub_panel.hide()
        elif self.sidebar_open:
            w = self.width()
            self.anim_side.setStartValue(self.sidebar.pos())
            self.anim_side.setEndValue(QPoint(w, 0))
            self.sidebar_open = False
            self.anim_side.start()
            self.anim_side.finished.connect(self._on_sidebar_closed)

    def _on_sidebar_closed(self):
        if not self.sidebar_open:
            self.sidebar.hide()
            self.shield.hide()
            try: self.anim_side.finished.disconnect(self._on_sidebar_closed)
            except: pass

    def update_geometries(self):
        """Calculates the 190px sidebar vs the Full-Screen remainder"""
        w, h = self.width(), self.height()
        
        # 1. Settings Panel takes everything left of the sidebar
        remainder_width = w - self.sidebar_width
        self.sub_panel.setGeometry(0, 0, remainder_width, h)
        
        # 2. Sidebar sticks to the right edge
        if self.sidebar_open:
            self.sidebar.setGeometry(w - self.sidebar_width, 0, self.sidebar_width, h)
        else:
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)

    def resizeEvent(self, event):
        w, h = self.width(), self.height()
        self.settings_btn.move(w - 90, 10)
        self.shield.setGeometry(0, 0, w, h)
        self.update_geometries()
        super().resizeEvent(event)

    def update_panel(self, proc):
        if self.sub_open:
            self.sub_panel.update_state(proc)