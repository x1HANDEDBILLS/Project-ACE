from PySide6.QtWidgets import QWidget, QGridLayout, QPushButton, QVBoxLayout, QHBoxLayout, QLineEdit
from PySide6.QtCore import Qt, Signal
import theme

class TacticalKeyboard(QWidget):
    # Signal emitted when 'Enter' is pressed or text changes
    submitted = Signal(str)
    
    def __init__(self, initial_text="", parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(0, 0, 0, 0)
        self.layout.setSpacing(10)

        # 1. Preview Display
        self.display = QLineEdit(initial_text)
        self.display.setAlignment(Qt.AlignCenter)
        self.display.setReadOnly(True) # Only modified via buttons
        self.display.setStyleSheet(f"""
            background: rgba(0, 0, 0, 0.6);
            border: 1px solid {theme.ACTIVE['hex']};
            color: white;
            font-family: 'Consolas';
            font-size: 22px;
            padding: 10px;
            border-radius: 4px;
        """)
        self.layout.addWidget(self.display)

        # 2. Key Grid
        self.grid = QGridLayout()
        self.grid.setSpacing(5)
        
        # Layout Definition
        rows = [
            ['1','2','3','4','5','6','7','8','9','0'],
            ['Q','W','E','R','T','Y','U','I','O','P'],
            ['A','S','D','F','G','H','J','K','L','-'],
            ['Z','X','C','V','B','N','M', '.', '_'],
            ['CLR', 'SPACE', 'BACK', 'ENTER']
        ]

        for row_idx, row in enumerate(rows):
            row_widget = QHBoxLayout()
            for key in row:
                btn = QPushButton(key)
                btn.setFixedSize(self._get_key_size(key))
                btn.setStyleSheet(self._get_key_style(key))
                btn.clicked.connect(lambda ch=key: self._handle_key(ch))
                row_widget.addWidget(btn)
            self.layout.addLayout(row_widget)

    def _get_key_size(self, key):
        if key == 'SPACE': return (160, 45)
        if key in ['BACK', 'ENTER', 'CLR']: return (80, 45)
        return (45, 45)

    def _get_key_style(self, key):
        h = theme.ACTIVE['hex']
        bg = "rgba(40, 40, 40, 0.8)"
        if key == 'ENTER': bg = h; color = "black"
        else: color = "white"
        
        return f"""
            QPushButton {{
                background: {bg};
                color: {color};
                border: 1px solid rgba(255,255,255,0.1);
                font-family: 'Consolas';
                font-weight: bold;
                font-size: 14px;
                border-radius: 3px;
            }}
            QPushButton:pressed {{ background: {h}; color: black; }}
        """

    def _handle_key(self, key):
        curr = self.display.text()
        if key == 'ENTER': self.submitted.emit(curr)
        elif key == 'BACK': self.display.setText(curr[:-1])
        elif key == 'CLR': self.display.clear()
        elif key == 'SPACE': self.display.insert(" ")
        else: self.display.insert(key)

    def refresh_theme(self):
        # Update display border and Enter button color
        self.display.setStyleSheet(self.display.styleSheet().replace("border: 1px solid", f"border: 1px solid {theme.ACTIVE['hex']}"))