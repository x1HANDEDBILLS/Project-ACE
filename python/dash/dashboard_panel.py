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
        
        # 1. MAIN HUD GEAR (74x74)
        self.settings_btn = CustomButton("⚙", self)
        self.settings_btn.setFixedSize(74, 74)
        self.settings_btn.font_size = 42 
        self.settings_btn.edge_size = 12.0
        self.settings_btn.y_nudge = -4 
        self.settings_btn.clicked.connect(self.toggle_sidebar)

        # 2. MAIN SHIELD (Blocks interaction with the background)
        self.shield = QFrame(self)
        self.shield.hide()
        self.shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")

        # 3. SETTINGS PANEL (Nested inside the shield)
        self.settings_gui = SettingsPanel(self.shield)
        self.settings_gui.setFixedSize(834, 600)
        self.settings_gui.hide()

        # 4. POPOUT DEDICATED SHIELD
        self.popout_shield = QFrame(self)
        self.popout_shield.hide()
        self.popout_shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")
        self.popout_shield.setAttribute(Qt.WA_NoMousePropagation, False)
        self.popout_shield.mousePressEvent = lambda e: e.accept()

        # 5. THEME POPOUT
        self.theme_panel = TacticalPopout(self.popout_shield)
        self.theme_panel.hide()
        # FIX: Reset theme_open flag when popout closes itself
        self.theme_panel.closed_manually.connect(self._reset_theme_state)
        
        # 6. SIDEBAR
        self.sidebar_width = 190
        self.sidebar = QFrame(self)
        self.sidebar.setAttribute(Qt.WA_StyledBackground)
        self.sidebar.setStyleSheet(f"QFrame {{ background-color: rgba(5, 5, 5, 220); border-left: 2px solid {theme.ACTIVE['hex']}; }}")
        
        self.sidebar_container = QVBoxLayout(self.sidebar)
        self.sidebar_container.setContentsMargins(0, 0, 0, 0)
        self.button_grid = PanelButtons(self.sidebar)
        self.sidebar_container.addWidget(self.button_grid)
        
        # Logic connections
        self.button_grid.back_btn.clicked.connect(self.smart_back)
        self.button_grid.inner_settings_btn.clicked.connect(self.toggle_settings_gui)

        # State management
        self.sidebar_open = False
        self.settings_open = False
        self.theme_open = False
        
        self.anim_side = QPropertyAnimation(self.sidebar, b"pos")
        self.anim_side.setDuration(250)
        self.anim_side.setEasingCurve(QEasingCurve.OutCubic)
        # Connected once to prevent signal stacking and warnings
        self.anim_side.finished.connect(self._on_sidebar_closed)

    def _reset_theme_state(self):
        """Called by TacticalPopout signal to keep state in sync"""
        self.theme_open = False

    def update_panel(self, proc):
        if hasattr(self, 'settings_gui'):
            self.settings_gui.update_state(proc)

    def toggle_sidebar(self):
        w, h = self.width(), self.height()
        if not self.sidebar_open:
            self.shield.setGeometry(0, 0, w, h)
            self.shield.show()
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)
            self.sidebar.show()
            
            # --- Z-ORDER FIX ---
            self.shield.raise_() 
            self.settings_btn.raise_() # Gear above background shield
            self.sidebar.raise_()      # Sidebar above Gear
            
            self.anim_side.stop()
            self.anim_side.setStartValue(QPoint(w, 0))
            self.anim_side.setEndValue(QPoint(w - self.sidebar_width, 0))
            self.sidebar_open = True
            self.anim_side.start()
        else:
            self.smart_back()

    def toggle_settings_gui(self):
        """Method to handle inner settings button"""
        if not self.settings_open:
            self.settings_open = True
            self.settings_gui.show()
            self.settings_gui.raise_()
        else:
            self.settings_open = False
            self.settings_gui.hide()

    def toggle_theme_panel(self):
        """Triggers the popout containing the gradient logic"""
        w, h = self.width(), self.height()
        if not self.theme_open:
            self.theme_open = True
            self.popout_shield.setGeometry(0, 0, w, h)
            self.popout_shield.show()
            self.popout_shield.raise_() 
            
            pw, ph = self.theme_panel.width(), self.theme_panel.height()
            self.theme_panel.move((w - pw) // 2, (h - ph) // 2)
            self.theme_panel.show_hud()
            self.theme_panel.raise_()
        else:
            # Note: theme_open is also reset by closed_manually signal
            self.theme_open = False
            self.theme_panel.hide_hud()

    def smart_back(self):
        """Logic for the 'Back' button on the sidebar"""
        if self.theme_open:
            self.toggle_theme_panel()
        elif self.settings_open:
            self.settings_open = False
            self.settings_gui.hide()
        elif self.sidebar_open:
            w = self.width()
            self.anim_side.stop()
            self.anim_side.setEndValue(QPoint(w, 0))
            self.sidebar_open = False
            self.anim_side.start()

    def _on_sidebar_closed(self):
        """Hides the layers once the sidebar is physically off-screen"""
        if not self.sidebar_open:
            self.sidebar.hide()
            self.shield.hide()

    def resizeEvent(self, event):
        w, h = self.width(), self.height()
        self.settings_btn.move(w - self.settings_btn.width() - 10, 10)
        self.shield.setGeometry(0, 0, w, h)
        self.popout_shield.setGeometry(0, 0, w, h)
        
        if self.sidebar_open:
            self.sidebar.setGeometry(w - self.sidebar_width, 0, self.sidebar_width, h)
        else:
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)
            
        if event: super().resizeEvent(event)