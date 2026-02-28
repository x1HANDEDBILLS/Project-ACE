import os
import json
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QListWidget, QLabel
from widgets.button import CustomButton
import theme

class TunePage5(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(40, 20, 40, 20)

        # 1. PROFILE NAME INPUT
        self.name_label = QLabel("// ASSIGN PROFILE NAME:")
        self.name_input = QLineEdit()
        self.name_input.setPlaceholderText("ENTER_NAME_HERE...")
        self.name_input.setStyleSheet(f"""
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid {theme.ACTIVE['hex']};
            color: white;
            padding: 10px;
            font-family: 'Consolas';
        """)

        # 2. SAVE BUTTON
        self.save_btn = CustomButton("SAVE TO DISK", self)
        self.save_btn.clicked.connect(self.save_current_profile)

        # 3. EXISTING PROFILES LIST
        self.list_label = QLabel("// SAVED PROFILES:")
        self.profile_list = QListWidget()
        self.profile_list.setStyleSheet("background: rgba(0,0,0,0.3); color: white; border: none;")
        self.refresh_profile_list()

        # 4. SLOT ASSIGNMENT (1-4)
        self.slot_layout = QHBoxLayout()
        self.slot_buttons = []
        for i in range(1, 5):
            btn = CustomButton(f"SLOT {i}", self, small=True)
            btn.clicked.connect(lambda checked, s=i: self.load_to_slot(s))
            self.slot_layout.addWidget(btn)

        # Add to main layout
        self.layout.addWidget(self.name_label)
        self.layout.addWidget(self.name_input)
        self.layout.addWidget(self.save_btn)
        self.layout.addSpacing(20)
        self.layout.addWidget(self.list_label)
        self.layout.addWidget(self.profile_list)
        self.layout.addLayout(self.slot_layout)

    def save_current_profile(self):
        name = self.name_input.text().strip()
        if not name: return
        
        # Collect data from Page 1-4 (Implementation depends on your state manager)
        profile_data = {
            "name": name,
            "deadzones": [1500] * 12, # Replace with actual values from Pages 1-4
            "axis_map": [0,1,2,3,4,5,6,7,8,9,10,11],
            "inversions": [False] * 12
        }
        
        with open(f"profiles/{name}.json", "w") as f:
            json.dump(profile_data, f, indent=4)
        self.refresh_profile_list()

    def load_to_slot(self, slot_id):
        selected = self.profile_list.currentItem()
        if not selected: return
        profile_name = selected.text()
        
        # SEND COMMAND TO C++ ENGINE
        # This triggers the InputManager::loadProfile in your C++ code
        from shared.socket import GLOBAL_SOCKET
        GLOBAL_SOCKET.send(f"SET_SLOT|{slot_id}|{profile_name}.json")

    def refresh_profile_list(self):
        self.profile_list.clear()
        if not os.path.exists("profiles"): os.makedirs("profiles")
        for file in os.listdir("profiles"):
            if file.endswith(".json"):
                self.profile_list.addItem(file.replace(".json", ""))

    def refresh_theme(self):
        h = theme.ACTIVE['hex']
        style = f"color: {h}; font-family: 'Consolas'; font-weight: bold;"
        self.name_label.setStyleSheet(style)
        self.list_label.setStyleSheet(style)