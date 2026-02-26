from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QGridLayout, 
                             QLabel, QFrame)
from PySide6.QtCore import Qt

class InputReadPage5(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.main_layout = QVBoxLayout(self)
        self.main_layout.setContentsMargins(20, 20, 20, 20)

        # 1. Dashboard Title
        title = QLabel("MASTER PORT STATUS")
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("color: white; font-size: 24px; font-weight: bold; margin-bottom: 20px;")
        self.main_layout.addWidget(title)

        # 2. Grid for the 4 Port Monitors
        self.grid = QGridLayout()
        self.grid.setSpacing(15)
        self.main_layout.addLayout(self.grid)

        # Initialize Port Cards
        self.port_cards = {}
        port_configs = [
            ("1", "PORT 1", "#00FF00"),
            ("2", "PORT 2", "#FF5500"),
            ("3", "PORT 3", "#BB00FF"),
            ("4", "PORT 4", "#00E5FF")
        ]

        for i, (id, name, color) in enumerate(port_configs):
            card = self.create_port_card(name, color)
            self.grid.addWidget(card, i // 2, i % 2)
            self.port_cards[id] = {
                "frame": card,
                "name_lbl": card.findChild(QLabel, "name"),
                "dev_lbl": card.findChild(QLabel, "device"),
                "stat_lbl": card.findChild(QLabel, "status"),
                "color": color
            }

        self.main_layout.addStretch()

    def create_port_card(self, port_name, theme_color):
        """Creates a stylized visual card for each port."""
        frame = QFrame()
        frame.setStyleSheet(f"""
            QFrame {{
                background-color: #111;
                border: 2px solid #222;
                border-radius: 10px;
            }}
        """)
        layout = QVBoxLayout(frame)
        
        name_lbl = QLabel(port_name)
        name_lbl.setObjectName("name")
        name_lbl.setStyleSheet(f"color: {theme_color}; font-weight: bold; font-size: 16px; border: none;")
        
        dev_lbl = QLabel("NO DEVICE")
        dev_lbl.setObjectName("device")
        dev_lbl.setStyleSheet("color: #666; font-size: 11px; border: none;")
        
        stat_lbl = QLabel("OFFLINE")
        stat_lbl.setObjectName("status")
        stat_lbl.setAlignment(Qt.AlignCenter)
        stat_lbl.setStyleSheet("color: #444; font-family: 'Consolas'; font-size: 14px; margin-top: 10px; border: none;")

        layout.addWidget(name_lbl)
        layout.addWidget(dev_lbl)
        layout.addWidget(stat_lbl)
        return frame

    def update_state(self, proc):
        """Updates all 4 port status cards simultaneously."""
        if not proc: return
        
        try:
            slots = proc.get_slots()
            for id, UI in self.port_cards.items():
                data = slots.get(id)
                is_connected = data and data.get("conn", False)

                if is_connected:
                    UI["frame"].setStyleSheet(f"background-color: #151515; border: 2px solid {UI['color']}; border-radius: 10px;")
                    UI["dev_lbl"].setText(data.get('name', 'HID DEVICE').upper())
                    UI["dev_lbl"].setStyleSheet("color: #AAA; font-size: 11px; border: none;")
                    
                    # Mini-Telemetry Preview (Axis 0 and Button 0 status)
                    axes_count = len(data.get('axes', []))
                    btns_count = len(data.get('btns', []))
                    UI["stat_lbl"].setText(f"ACTIVE\n{axes_count} AXES | {btns_count} BTNS")
                    UI["stat_lbl"].setStyleSheet(f"color: {UI['color']}; font-weight: bold; border: none;")
                else:
                    UI["frame"].setStyleSheet("background-color: #050505; border: 2px solid #222; border-radius: 10px;")
                    UI["dev_lbl"].setText("NO DEVICE")
                    UI["dev_lbl"].setStyleSheet("color: #444; font-size: 11px; border: none;")
                    UI["stat_lbl"].setText("OFFLINE")
                    UI["stat_lbl"].setStyleSheet("color: #333; border: none;")

        except Exception:
            pass