from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QGridLayout, 
                                QLabel, QProgressBar, QScrollArea, QFrame)
from PySide6.QtCore import Qt

class InputReadPage1(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.main_layout = QVBoxLayout(self)
        self.main_layout.setContentsMargins(20, 20, 20, 20)

        # 1. Header Status Display - PORT 1
        self.status_label = QLabel("PORT 1: WAITING...")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("""
            color: #555; 
            font-family: 'Consolas', monospace; 
            font-size: 20px; 
            font-weight: bold;
            margin-bottom: 10px;
        """)
        self.main_layout.addWidget(self.status_label)

        # 2. Scrollable Area
        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll.setStyleSheet("background: transparent; border: none;")
        
        self.container_widget = QWidget()
        self.container_layout = QVBoxLayout(self.container_widget)
        self.scroll.setWidget(self.container_widget)
        self.main_layout.addWidget(self.scroll)

        # Trackers
        self.axis_bars = []
        self.btn_indicators = []
        self.accel_bars = []  
        self.gyro_bars = []   
        self.current_device = None
        self.btn_states = []

    def get_bar_style(self, chunk_color="#00FF00"):
        """Centralized style for all progress bars."""
        return f"""
            QProgressBar {{
                border: 1px solid #333;
                background: #111;
                height: 18px;
                color: #AAA; 
                font-family: 'Consolas';
                font-size: 10px;
                text-align: center;
                border-radius: 2px;
            }}
            QProgressBar::chunk {{ background-color: {chunk_color}; }}
        """

    def create_motion_bar(self, label_text, min_range, max_range, color="#00FFCC"):
        """Helper for Accel and Gyro bars."""
        container = QHBoxLayout()
        lbl = QLabel(label_text)
        lbl.setFixedWidth(60)
        lbl.setStyleSheet("color: #777; font-family: 'Consolas'; font-size: 10px; border: none; font-weight: bold;")
        
        bar = QProgressBar()
        bar.setRange(min_range, max_range) 
        bar.setTextVisible(True)
        bar.setAlignment(Qt.AlignCenter)
        bar.setStyleSheet(self.get_bar_style(color))
        
        container.addWidget(lbl)
        container.addWidget(bar)
        return container, bar

    def create_widgets(self, num_axes, num_btns, has_motion):
        """Rebuilds UI with 16-bit range support for Gyro."""
        while self.container_layout.count():
            item = self.container_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        
        self.axis_bars = []
        self.btn_indicators = []
        self.accel_bars = []
        self.gyro_bars = []
        self.btn_states = [False] * num_btns

        # --- ANALOG HARDWARE AXES (Sticks/Triggers) ---
        ax_header = QLabel("ANALOG HARDWARE AXES")
        ax_header.setStyleSheet("color: #555; font-weight: bold; font-size: 10px; margin-top: 5px;")
        self.container_layout.addWidget(ax_header)

        for i in range(num_axes):
            bar = QProgressBar()
            bar.setRange(-32768, 32767) 
            bar.setTextVisible(True)
            bar.setAlignment(Qt.AlignCenter)
            bar.setFormat(f"AXIS {i}: %v")
            bar.setStyleSheet(self.get_bar_style("#00FF00"))
            self.container_layout.addWidget(bar)
            self.axis_bars.append(bar)

        # --- MOTION DATA (ACCEL & GYRO) ---
        if has_motion:
            self.container_layout.addSpacing(10)
            mot_header = QLabel("MOTION TELEMETRY")
            mot_header.setStyleSheet("color: #555; font-weight: bold; font-size: 10px;")
            self.container_layout.addWidget(mot_header)

            motion_box = QFrame()
            motion_box.setStyleSheet("background: #0a0a0a; border: 1px solid #222; border-radius: 4px;")
            motion_vbox = QVBoxLayout(motion_box)
            
            # ACCEL (Normalized -1.0 to 1.0 mapped to -100 to 100)
            for label in ["ACC X", "ACC Y", "ACC Z"]:
                lay, bar = self.create_motion_bar(label, -100, 100, "#00FFCC")
                bar.setFormat("%v%")
                motion_vbox.addLayout(lay)
                self.accel_bars.append(bar)
            
            line = QFrame()
            line.setFrameShape(QFrame.HLine)
            line.setStyleSheet("color: #222;")
            motion_vbox.addWidget(line)

            # GYRO (16-bit raw mapping: hit max at 90 degrees)
            # Labels reflect your RC use case (Pitch, Yaw, Roll)
            gyro_labels = ["PITCH", "YAW RT", "ROLL"]
            for label in gyro_labels:
                lay, bar = self.create_motion_bar(label, -32768, 32767, "#00CCFF")
                bar.setFormat("%v")
                motion_vbox.addLayout(lay)
                self.gyro_bars.append(bar)

            self.container_layout.addWidget(motion_box)

        # --- DIGITAL BUTTONS ---
        self.container_layout.addSpacing(10)
        btn_header = QLabel("DIGITAL BUTTONS")
        btn_header.setStyleSheet("color: #555; font-weight: bold; font-size: 10px;")
        self.container_layout.addWidget(btn_header)

        btn_container = QWidget()
        btn_grid = QGridLayout(btn_container)
        btn_grid.setContentsMargins(0, 5, 0, 5)
        btn_grid.setSpacing(6)
        
        for i in range(num_btns):
            lbl = QLabel(str(i))
            lbl.setFixedSize(28, 28)
            lbl.setAlignment(Qt.AlignCenter)
            lbl.setStyleSheet("background: #222; color: #444; border-radius: 4px; font-weight: bold; font-size: 10px;")
            btn_grid.addWidget(lbl, i // 8, i % 8)
            self.btn_indicators.append(lbl)
        
        self.container_layout.addWidget(btn_container)
        self.container_layout.addStretch()

    def update_state(self, proc):
        if not proc or not self.isVisible(): 
            return

        try:
            slots = proc.get_slots() 
            p1 = slots.get("1") 
            if not (p1 and p1.get("conn", False)):
                if self.current_device:
                    self.status_label.setText("PORT 1: WAITING...")
                    self.status_label.setStyleSheet("color: #555; font-size: 20px; font-weight: bold;")
                    self.create_widgets(0, 0, False)
                    self.current_device = None
                return

            device_name = p1.get('name', 'PS4 Controller')
            raw_axes = p1.get('raw', [])
            btns = p1.get('btns', [])
            accel = p1.get('accel', [])
            gyro = p1.get('gyro', [])

            footprint = f"{device_name}_{len(raw_axes)}_{len(btns)}"
            if self.current_device != footprint:
                self.status_label.setText(f"PORT 1: {device_name.upper()}")
                self.status_label.setStyleSheet("color: #00FF00; font-family: 'Consolas'; font-size: 20px; font-weight: bold;")
                self.create_widgets(len(raw_axes), len(btns), (len(accel) > 0))
                self.current_device = footprint

            # Update Raw Sticks
            for i, val in enumerate(raw_axes):
                if i < len(self.axis_bars): self.axis_bars[i].setValue(int(val))

            # Update Accel (Convert float to percentage for the bar)
            for i, val in enumerate(accel):
                if i < len(self.accel_bars): 
                    self.accel_bars[i].setValue(int(float(val) * 100))

            # Update Gyro (Direct 16-bit Integer)
            for i, val in enumerate(gyro):
                if i < len(self.gyro_bars): 
                    self.gyro_bars[i].setValue(int(val))

            # Update Buttons
            for i, state in enumerate(btns):
                if i < len(self.btn_indicators):
                    is_pressed = bool(state)
                    if is_pressed != self.btn_states[i]:
                        self.btn_states[i] = is_pressed
                        color = "#00FF00" if is_pressed else "#222"
                        text_color = "#000" if is_pressed else "#444"
                        self.btn_indicators[i].setStyleSheet(f"background: {color}; color: {text_color}; border-radius: 4px; font-weight: bold;")
                    
        except Exception as e:
            print(f"UI Update Error: {e}")