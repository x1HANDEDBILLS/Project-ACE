from PySide6.QtWidgets import QWidget, QGridLayout, QPushButton, QVBoxLayout, QLineEdit
from PySide6.QtCore import Qt, Signal
import theme

class TacticalKeypad(QWidget):
    """
    A touch-optimized numerical keypad designed to fit inside 
    a TacticalPopout content area.
    """
    # Signal emitted when 'ENTER' is pressed
    submitted = Signal(str)
    
    def __init__(self, initial_text="", parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(0, 0, 0, 0)
        self.layout.setSpacing(10)

        # 1. Preview Display (Shows what the user is typing)
        self.display = QLineEdit(str(initial_text))
        self.display.setAlignment(Qt.AlignCenter)
        self.display.setReadOnly(True)
        self.refresh_display_style()
        self.layout.addWidget(self.display)

        # 2. Key Grid
        self.grid = QGridLayout()
        self.grid.setSpacing(6)
        
        # Numpad Layout optimized for Deadzone percentages (0-100)
        keys = [
            ['7', '8', '9'],
            ['4', '5', '6'],
            ['1', '2', '3'],
            ['CLR', '0', 'BACK'],
            ['ENTER']
        ]

        for r, row in enumerate(keys):
            for c, key in enumerate(row):
                btn = QPushButton(key)
                
                # 'ENTER' button spans the entire bottom row
                col_span = 3 if key == 'ENTER' else 1
                
                btn.setFixedHeight(50)
                btn.setStyleSheet(self._get_key_style(key))
                
                # FIX: Added '_' to catch the boolean 'checked' signal from the click event
                # This prevents the TypeError: can only concatenate str (not "bool") to str
                btn.clicked.connect(lambda _, k=key: self._on_key_press(k))
                
                self.grid.addWidget(btn, r, c, 1, col_span)

        self.layout.addLayout(self.grid)

    def _get_key_style(self, key):
        h = theme.ACTIVE['hex']
        bg = "rgba(45, 45, 45, 0.9)"
        txt = "white"
        
        if key == 'ENTER':
            bg = h
            txt = "black"
        
        return f"""
            QPushButton {{
                background: {bg};
                color: {txt};
                border: 1px solid rgba(255,255,255,0.1);
                font-family: 'Consolas';
                font-weight: bold;
                font-size: 16px;
                border-radius: 4px;
            }}
            QPushButton:pressed {{
                background: {h};
                color: black;
            }}
        """

    def _on_key_press(self, key):
        curr = self.display.text()
        
        if key == 'ENTER':
            # Send result back to the popout
            self.submitted.emit(curr)
            
        elif key == 'BACK':
            # Remove last character
            self.display.setText(curr[:-1])
            
        elif key == 'CLR':
            # Clear everything
            self.display.clear()
            
        else:
            # Numerical input: Limit to 3 digits (max 100 for deadzones)
            if len(curr) < 3:
                # Ensure we only handle string addition
                self.display.setText(curr + str(key))

    def refresh_display_style(self):
        """Helper to set/update the display styling based on active theme."""
        h = theme.ACTIVE['hex']
        self.display.setStyleSheet(f"""
            QLineEdit {{
                background: rgba(0, 0, 0, 0.6);
                border: 1px solid {h};
                color: white;
                font-family: 'Consolas';
                font-size: 24px;
                padding: 8px;
                border-radius: 4px;
            }}
        """)
                
    def refresh_theme(self):
        """Updates the keypad visuals live if the theme changes."""
        self.refresh_display_style()
        # Refresh all buttons in the grid to update colors
        for i in range(self.grid.count()):
            widget = self.grid.itemAt(i).widget()
            if isinstance(widget, QPushButton):
                widget.setStyleSheet(self._get_key_style(widget.text()))