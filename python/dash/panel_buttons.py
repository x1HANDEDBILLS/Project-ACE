from PySide6.QtWidgets import QWidget, QGridLayout
from PySide6.QtCore import Qt
from widgets.button import CustomButton
import theme

class PanelButtons(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.layout = QGridLayout(self)
        
        # Grid math for 190px sidebar
        # margins: left=14 to center the 74px buttons in the 190px width
        self.layout.setContentsMargins(14, 10, 10, 10)
        self.layout.setSpacing(14)
        
        self.buttons = []
        
        # We generate 14 slots (7 rows, 2 columns)
        for i in range(14):
            row = i // 2
            col = i % 2
            
            btn = CustomButton("", self)
            btn.setFixedSize(74, 74)
            btn.font_size = 42 # Default for icons
            btn.edge_size = 12.0
            
            # --- INDEX 1: BACK / CLOSE ---
            if i == 1:
                btn.setText("→") 
                self.back_btn = btn
            
            # --- INDEX 10: INPUT TUNE (One row above Input) ---
            elif i == 10:
                btn.setText("Input\nTune")
                btn.font_size = 14  # Smaller font for 2-line text
                btn.y_nudge = 0     # Reset nudge for text alignment
                self.tune_btn = btn

            # --- INDEX 12: INPUT MONITOR (Bottom Left) ---
            elif i == 12:
                btn.setText("Input")
                btn.font_size = 18 
                self.input_btn = btn
            
            # --- INDEX 13: SETTINGS (Bottom Right) ---
            elif i == 13:
                btn.setText("⚙")
                btn.font_size = 42
                self.inner_settings_btn = btn
            
            # Add to grid and list
            self.layout.addWidget(btn, row, col)
            self.buttons.append(btn)
            
        self.setLayout(self.layout)
        self.refresh_theme()

    def refresh_theme(self):
        """Ensures all buttons repaint with the latest theme.ACTIVE color"""
        for btn in self.buttons:
            if hasattr(btn, 'update'):
                btn.update()

    def update_state(self, proc):
        """Placeholder for potential button state animations based on C++ telemetry"""
        pass