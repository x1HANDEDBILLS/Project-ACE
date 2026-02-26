from PySide6.QtWidgets import QWidget, QFrame, QVBoxLayout, QLabel
from PySide6.QtCore import Qt, QPropertyAnimation, QEasingCurve, QPoint, QRect
from widgets.button import CustomButton 
from dash.panel_buttons import PanelButtons
from widgets.popout import TacticalPopout 
from dash.settings import SettingsPanel 
from input.input_read import InputPanel 
from input.input_tune import InputTunePanel # <--- NEW IMPORT
import theme

class DashboardPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # 1. MAIN HUD GEAR (The permanent floating button)
        self.settings_btn = CustomButton("⚙", self)
        self.settings_btn.setFixedSize(74, 74)
        self.settings_btn.font_size = 42 
        self.settings_btn.edge_size = 12.0
        self.settings_btn.y_nudge = -4 
        self.settings_btn.clicked.connect(self.toggle_sidebar)

        # 2. MAIN SHIELD (The dimming layer for all large panels)
        self.shield = QFrame(self)
        self.shield.hide()
        self.shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")

        # 3. PANELS (Skeletons)
        self.settings_gui = SettingsPanel(self.shield)
        self.settings_gui.setFixedSize(834, 600)
        self.settings_gui.hide()
        
        self.input_gui = InputPanel(self.shield)
        self.input_gui.setFixedSize(834, 600)
        self.input_gui.hide()

        self.tune_gui = InputTunePanel(self.shield) # <--- NEW INITIALIZATION
        self.tune_gui.setFixedSize(834, 600)
        self.tune_gui.hide()

        # 4. POPOUT DEDICATED SHIELD
        self.popout_shield = QFrame(self)
        self.popout_shield.hide()
        self.popout_shield.setStyleSheet("background-color: rgba(0, 0, 0, 0);")
        self.popout_shield.setAttribute(Qt.WA_NoMousePropagation, False)
        self.popout_shield.mousePressEvent = lambda e: e.accept()

        # 5. THEME POPOUT
        self.theme_panel = TacticalPopout(self.popout_shield)
        self.theme_panel.hide()
        self.theme_panel.closed_manually.connect(self._reset_theme_state)
        
        # 6. SIDEBAR
        self.sidebar_width = 190
        self.sidebar = QFrame(self)
        self.sidebar.setAttribute(Qt.WA_StyledBackground)
        self.apply_sidebar_style()
        
        self.sidebar_container = QVBoxLayout(self.sidebar)
        self.sidebar_container.setContentsMargins(0, 0, 0, 0)
        self.button_grid = PanelButtons(self.sidebar)
        self.sidebar_container.addWidget(self.button_grid)
        
        # --- BUTTON CONNECTIONS ---
        self.button_grid.back_btn.clicked.connect(self.smart_back)
        self.button_grid.inner_settings_btn.clicked.connect(self.toggle_settings_gui)
        self.button_grid.input_btn.clicked.connect(self.toggle_input_gui)
        self.button_grid.tune_btn.clicked.connect(self.toggle_tune_gui) # <--- NEW CONNECTION

        self.sidebar_open = False
        self.settings_open = False
        self.input_open = False 
        self.tune_open = False # <--- NEW STATE
        self.theme_open = False
        
        self.anim_side = QPropertyAnimation(self.sidebar, b"pos")
        self.anim_side.setDuration(250)
        self.anim_side.setEasingCurve(QEasingCurve.OutCubic)
        self.anim_side.finished.connect(self._on_sidebar_closed)

    def apply_sidebar_style(self):
        self.sidebar.setStyleSheet(f"""
            QFrame {{ 
                background-color: rgba(5, 5, 5, 220); 
                border-left: 2px solid {theme.ACTIVE['hex']}; 
            }}
        """)

    def refresh_theme(self):
        self.apply_sidebar_style()
        self.settings_btn.update()
        if hasattr(self.button_grid, 'refresh_theme'): self.button_grid.refresh_theme()
        if hasattr(self.settings_gui, 'refresh_theme'): self.settings_gui.refresh_theme()
        if hasattr(self.input_gui, 'refresh_theme'): self.input_gui.refresh_theme()
        if hasattr(self.tune_gui, 'refresh_theme'): self.tune_gui.refresh_theme() # <--- NEW REFRESH
        self.update()

    def update_panel(self, proc):
        if self.settings_open: self.settings_gui.update_state(proc)
        if self.input_open: self.input_gui.update_state(proc)
        if self.tune_open: self.tune_gui.update_state(proc) # <--- DATA FLOW FOR TUNE

    def update_panel_positions(self):
        w, h = self.width(), self.height()
        available_w = w - self.sidebar_width if self.sidebar_open else w
        target_x = (available_w - 834) // 2
        target_y = (h - 600) // 2
        
        if self.settings_gui: self.settings_gui.move(target_x, target_y)
        if self.input_gui: self.input_gui.move(target_x, target_y)
        if self.tune_gui: self.tune_gui.move(target_x, target_y) # <--- NEW POSITIONING

    def toggle_sidebar(self):
        w, h = self.width(), self.height()
        if not self.sidebar_open:
            self.shield.setGeometry(0, 0, w, h)
            self.shield.show()
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)
            self.sidebar.show()
            self.shield.raise_() 
            self.settings_btn.raise_()
            self.sidebar.raise_()
            self.anim_side.stop()
            self.anim_side.setStartValue(QPoint(w, 0))
            self.anim_side.setEndValue(QPoint(w - self.sidebar_width, 0))
            self.sidebar_open = True
            self.anim_side.start()
            self.update_panel_positions()
        else:
            self.smart_back()

    def toggle_settings_gui(self):
        if not self.settings_open:
            if self.input_open: self.toggle_input_gui()
            if self.tune_open: self.toggle_tune_gui()
            self.settings_open = True
            self.settings_gui.show()
            self.settings_gui.raise_()
        else:
            self.settings_open = False
            self.settings_gui.hide()
        self.update_panel_positions()

    def toggle_input_gui(self):
        if not self.input_open:
            if self.settings_open: self.toggle_settings_gui()
            if self.tune_open: self.toggle_tune_gui()
            self.input_open = True
            self.input_gui.show()
            self.input_gui.raise_()
        else:
            self.input_open = False
            self.input_gui.hide()
        self.update_panel_positions()

    def toggle_tune_gui(self):
        """ Toggles the Input Tuning panel with mutual exclusion """
        if not self.tune_open:
            if self.settings_open: self.toggle_settings_gui()
            if self.input_open: self.toggle_input_gui()
            self.tune_open = True
            self.tune_gui.show()
            self.tune_gui.raise_()
        else:
            self.tune_open = False
            self.tune_gui.hide()
        self.update_panel_positions()

    def smart_back(self):
        if self.theme_open:
            self.toggle_theme_panel()
        elif self.tune_open:
            self.tune_open = False
            self.tune_gui.hide()
        elif self.input_open:
            self.input_open = False
            self.input_gui.hide()
        elif self.settings_open:
            self.settings_open = False
            self.settings_gui.hide()
        elif self.sidebar_open:
            w = self.width()
            self.anim_side.stop()
            self.anim_side.setEndValue(QPoint(w, 0))
            self.sidebar_open = False
            self.anim_side.start()
        self.update_panel_positions()

    def _on_sidebar_closed(self):
        if not self.sidebar_open:
            self.sidebar.hide()
            self.shield.hide()

    def resizeEvent(self, event):
        w, h = self.width(), self.height()
        self.update_panel_positions()
        self.settings_btn.move(w - self.settings_btn.width() - 10, 10)
        self.shield.setGeometry(0, 0, w, h)
        self.popout_shield.setGeometry(0, 0, w, h)
        if self.sidebar_open:
            self.sidebar.setGeometry(w - self.sidebar_width, 0, self.sidebar_width, h)
        else:
            self.sidebar.setGeometry(w, 0, self.sidebar_width, h)
        if event: super().resizeEvent(event)

    def _reset_theme_state(self):
        self.theme_open = False

    def toggle_theme_panel(self):
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
            self.theme_open = False
            self.theme_panel.hide_hud()