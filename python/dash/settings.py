from PySide6.QtWidgets import QFrame, QVBoxLayout, QLabel
from PySide6.QtCore import Qt
import theme

class SettingsPanel(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.setAttribute(Qt.WA_StyledBackground)
        # Main background (Nearly solid black)
        self.setStyleSheet(f"""
            QFrame#MainSettings {{ 
                background-color: rgba(10, 10, 10, 245); 
                border: none;
            }}
        """)
        self.setObjectName("MainSettings")
        
        # 1. Outer layout to create the 10px buffer
        self.outer_layout = QVBoxLayout(self)
        self.outer_layout.setContentsMargins(10, 10, 10, 10)
        
        # 2. The Internal Frame (The one with the border)
        self.content_frame = QFrame()
        self.content_frame.setStyleSheet(f"""
            QFrame {{
                background-color: rgba(20, 20, 20, 150);
                border: 2px solid {theme.ACTIVE['hex']};
                border-radius: 5px;
            }}
            QLabel {{
                color: {theme.ACTIVE['hex']};
                font-family: 'Consolas';
                font-size: 28px;
                font-weight: bold;
                border: none; /* Keep label from inheriting frame border */
            }}
        """)
        
        # 3. Inner layout for the actual settings content
        self.inner_layout = QVBoxLayout(self.content_frame)
        self.inner_layout.setContentsMargins(30, 30, 30, 30)
        
        self.header = QLabel("SYSTEM CONFIGURATION")
        self.inner_layout.addWidget(self.header)
        self.inner_layout.addStretch()
        
        # Add the bordered frame to the main panel
        self.outer_layout.addWidget(self.content_frame)
        
    def update_state(self, proc):
        """Handle real-time data if settings need to show CPU temp/Hz etc."""
        pass