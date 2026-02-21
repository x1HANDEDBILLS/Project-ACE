from PySide6.QtWidgets import QWidget, QGridLayout
from widgets.button import CustomButton

class PanelButtons(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.layout = QGridLayout(self)
        
        # New Math for 190px sidebar: 14px margins/spacing perfectly centers 74px buttons
        self.layout.setContentsMargins(14, 10, 10, 10)
        self.layout.setSpacing(14)
        
        self.buttons = []
        
        for i in range(14):
            row = i // 2
            col = i % 2
            
            btn = CustomButton("", self)
            btn.setFixedSize(74, 74)
            btn.font_size = 42
            btn.edge_size = 12.0
            
            if i == 1:
                # Use a standard arrow or adjust the string for the custom painter
                btn.setText("→") 
                self.back_btn = btn
            elif i == 13:
                btn.setText("⚙")
                self.inner_settings_btn = btn
            
            self.layout.addWidget(btn, row, col)
            self.buttons.append(btn)
            
        self.setLayout(self.layout)