from PySide6.QtWidgets import QWidget, QFrame, QVBoxLayout, QLabel
from PySide6.QtCore import Qt, QPropertyAnimation, QEasingCurve, QPoint, QRect
from widgets.button import CustomButton 
from dash.panel_buttons import PanelButtons
from widgets.popout import TacticalPopout 
from dash.settings import SettingsPanel 
import theme

class DashboardPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # 1. MAIN HUD GEAR
        self.settings_btn = CustomButton("⚙", self)
        self.settings_btn.setFixedSize(74, 74)
        self.settings_btn.font_size = 42 
        self.settings_btn.edge_size = 12.0
        self.settings_btn.y_nudge = -4 
        self.settings_btn.clicked.connect(self.toggle_sidebar)

        # 2. SHIELD (Overlay)
        self.shield = QFrame(self)
        self.shield.hide()
        self.shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")
        # Removed the mousePressEvent that closes everything on touch

        # 3. SETTINGS PANEL (Fixed Side-Filler)
        self.settings_gui = SettingsPanel(self.shield)
        # 834 width fills the gap next to the 190px sidebar
        self.settings_gui.setFixedSize(834, 600)
        self.settings_gui.hide()

        # 4. THEME SUB-POPOUT (Animated HUD - stays as a popout)
        self.theme_panel = TacticalPopout(self.shield)
        self.theme_panel.hide()
        theme_label = QLabel("SELECT SYSTEM COLOR\n\n[ AMBER ]\n[ CYAN ]\n[ RED ]")
        theme_label.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-family: 'Consolas'; font-size: 14px;")
        theme_label.setAlignment(Qt.AlignCenter)
        self.theme_panel.main_layout.insertWidget(1, theme_label)

        # 5. SIDEBAR
        self.sidebar_width = 190
        self.sidebar = QFrame(self)
        self.sidebar.setAttribute(Qt.WA_StyledBackground)
        self.sidebar.setStyleSheet(f"QFrame {{ background-color: rgba(5, 5, 5, 220); border-left: 2px solid {theme.ACTIVE['hex']}; }}")
        
        self.sidebar_container = QVBoxLayout(self.sidebar)
        self.sidebar_container.setContentsMargins(0, 0, 0, 0)
        self.button_grid = PanelButtons(self.sidebar)
        self.sidebar_container.addWidget(self.button_grid)
        
        self.button_grid.back_btn.clicked.connect(self.smart_back)
        self.button_grid.inner_settings_btn.clicked.connect(self.toggle_settings_gui)

        # Animation State
        self.sidebar_open = False
        self.settings_open = False
        self.theme_open = False
        self.anim_side = QPropertyAnimation(self.sidebar, b"pos")
        self.anim_side.setDuration(250)
        self.anim_side.setEasingCurve(QEasingCurve.OutCubic)

    def update_panel(self, proc):
        if hasattr(self, 'settings_gui'):
            self.settings_gui.update_state(proc)

    def toggle_sidebar(self):
        w, h = self.width(), self.height()
        if not self.sidebar_open:
            self.shield.setGeometry(0, 0, w, h)
            self.shield.show()
            self.sidebar.show()
            self.shield.raise_() 
            self.sidebar.raise_()
            
            self.anim_side.stop()
            self.anim_side.setStartValue(QPoint(w, 0))
            self.anim_side.setEndValue(QPoint(w - self.sidebar_width, 0))
            self.sidebar_open = True
            self.anim_side.start()
        else:
            self.smart_back()

    def toggle_settings_gui(self):
        if not self.settings_open:
            self.settings_open = True
            # Positioned at 0,0 to fill the left side
            self.settings_gui.move(0, 0)
            self.settings_gui.show()
            self.settings_gui.raise_()
        else:
            self.settings_open = False
            self.settings_gui.hide()

    def toggle_theme_panel(self):
        if not self.theme_open:
            self.theme_open = True
            # Center the theme popout specifically within the 834px settings area
            target_x = (834 - self.theme_panel.width()) // 2
            target_y = (600 - self.theme_panel.height()) // 2
            self.theme_panel.move(target_x, target_y)
            self.theme_panel.show_hud()
        else:
            self.theme_open = False
            self.theme_panel.hide_hud()

    def smart_back(self):
        if self.theme_open:
            self.theme_open = False
            self.theme_panel.hide_hud()
        elif self.settings_open:
            self.settings_open = False
            self.settings_gui.hide()
        elif self.sidebar_open:
            w = self.width()
            self.anim_side.stop()
            self.anim_side.setStartValue(self.sidebar.pos())
            self.anim_side.setEndValue(QPoint(w, 0))
            self.sidebar_open = False
            self.anim_side.finished.connect(self._on_sidebar_closed)
            self.anim_side.start()

    def _on_sidebar_closed(self):
        if not self.sidebar_open:
            self.sidebar.hide()
            self.shield.hide()
            try: self.anim_side.finished.disconnect(self._on_sidebar_closed)
            except: pass

    def resizeEvent(self, event):
        w, h = self.width(), self.height()
        btn_w = self.settings_btn.width()
        self.settings_btn.move(w - btn_w - 10, 10)
        self.shield.setGeometry(0, 0, w, h)
        
        if self.sidebar_open:
            self.sidebar.setGeometry(w - self.sidebar_width, 0, self.sidebar_width, h)
        else:
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)
            
        super().resizeEvent(event)