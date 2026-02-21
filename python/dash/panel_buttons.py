from PySide6.QtWidgets import QWidget, QGridLayout
from widgets.button import CustomButton

class PanelButtons(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.layout = QGridLayout(self)
        self.layout.setContentsMargins(10, 10, 10, 10)
        self.layout.setSpacing(10)
        
        self.buttons = []
        
        for i in range(14):
            row = i // 2
            col = i % 2
            
            # Create the button
            btn = CustomButton("", self)
            btn.setFixedSize(80, 80)
            btn.font_size = 42
            btn.edge_size = 12.0
            
            # Assign specific roles based on position
            if i == 1:  # Top Right
                btn.setText("→")
                self.back_btn = btn
            elif i == 13:  # Bottom Right
                btn.setText("⚙")
                self.inner_settings_btn = btn
            
            self.layout.addWidget(btn, row, col)
            self.buttons.append(btn)
            
        self.setLayout(self.layout)