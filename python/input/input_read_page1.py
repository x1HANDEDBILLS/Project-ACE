from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QGridLayout, 
                             QLabel, QProgressBar, QScrollArea, QFrame)
from PySide6.QtCore import Qt

class InputReadPage1(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        # Main Layout setup
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

        # 2. Scrollable Area for Dynamic Content
        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll.setStyleSheet("background: transparent; border: none;")
        
        self.container_widget = QWidget()
        self.container_layout = QVBoxLayout(self.container_widget)
        self.scroll.setWidget(self.container_widget)
        
        self.main_layout.addWidget(self.scroll)

        # Trackers for UI state
        self.axis_bars = []
        self.btn_indicators = []
        self.accel_bars = []  # List for X, Y, Z bars
        self.gyro_bars = []   # List for Pitch, Roll, Yaw bars
        self.current_device = None
        self.btn_states = []

    def create_motion_bar(self, label_text, min_range, max_range):
        """Helper to create a label and progress bar pair for motion data with specific ranges."""
        container = QHBoxLayout()
        lbl = QLabel(label_text)
        lbl.setFixedWidth(50)
        lbl.setStyleSheet("color: #00FFCC; font-family: 'Consolas'; font-size: 10px; border: none;")
        
        bar = QProgressBar()
        # Set range based on C++ scaling (e.g., Accel ~3340 per G, Gyro up to 16-bit)
        bar.setRange(min_range, max_range) 
        bar.setTextVisible(False)
        bar.setStyleSheet("""
            QProgressBar {
                border: 1px solid #222;
                background: #050505;
                height: 12px;
                border-radius: 2px;
            }
            QProgressBar::chunk { background-color: #00FFCC; }
        """)
        container.addWidget(lbl)
        container.addWidget(bar)
        return container, bar

    def create_widgets(self, num_axes, num_btns, has_motion):
        """Rebuilds UI with full hierarchy: RAW Group -> Component Sub-sections."""
        while self.container_layout.count():
            item = self.container_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        
        self.axis_bars = []
        self.btn_indicators = []
        self.accel_bars = []
        self.gyro_bars = []
        self.btn_states = [False] * num_btns

        # --- GLOBAL GROUP: RAW DATA ---
        raw_group_header = QLabel("--- RAW HARDWARE INPUTS ---")
        raw_group_header.setAlignment(Qt.AlignCenter)
        raw_group_header.setStyleSheet("color: #00FF00; font-weight: bold; font-size: 12px; margin-top: 5px; margin-bottom: 5px;")
        self.container_layout.addWidget(raw_group_header)

        # --- SUB-SECTION: ANALOG AXES (16-bit resolution) ---
        if num_axes > 0:
            ax_header = QLabel("ANALOG AXES")
            ax_header.setStyleSheet("color: #777; font-weight: bold; font-size: 10px; margin-top: 5px;")
            self.container_layout.addWidget(ax_header)

            for i in range(num_axes):
                bar = QProgressBar()
                # UPDATED: Matches C++ Sint16 range (-32768 to 32767)
                bar.setRange(-32768, 32767) 
                bar.setTextVisible(True)
                bar.setFormat(f"AXIS {i}: %v")
                bar.setStyleSheet("""
                    QProgressBar {
                        border: 1px solid #333;
                        background: #111;
                        height: 18px;
                        color: white;
                        font-size: 10px;
                        text-align: center;
                        border-radius: 2px;
                    }
                    QProgressBar::chunk { background-color: #00FF00; }
                """)
                self.container_layout.addWidget(bar)
                self.axis_bars.append(bar)

        # --- SUB-SECTION: MOTION TELEMETRY ---
        if has_motion:
            self.container_layout.addSpacing(10)
            mot_header = QLabel("MOTION TELEMETRY (ACCEL / GYRO)")
            mot_header.setStyleSheet("color: #777; font-weight: bold; font-size: 10px;")
            self.container_layout.addWidget(mot_header)

            motion_box = QFrame()
            motion_box.setStyleSheet("background: #111; border: 1px solid #222; border-radius: 4px;")
            motion_vbox = QVBoxLayout(motion_box)
            
            # Create 3 Accel Bars (Scaled for ~2.5G visibility: 3340 * 2.5)
            for label in ["ACC X", "ACC Y", "ACC Z"]:
                lay, bar = self.create_motion_bar(label, -8500, 8500)
                motion_vbox.addLayout(lay)
                self.accel_bars.append(bar)
            
            line = QFrame()
            line.setFrameShape(QFrame.HLine)
            line.setStyleSheet("color: #222;")
            motion_vbox.addWidget(line)

            # Create 3 Gyro Bars (Scaled for high-speed 16-bit rotation)
            for label in ["GYR P", "GYR R", "GYR Y"]:
                lay, bar = self.create_motion_bar(label, -32768, 32767)
                motion_vbox.addLayout(lay)
                self.gyro_bars.append(bar)

            self.container_layout.addWidget(motion_box)

        # --- SUB-SECTION: DIGITAL BUTTONS ---
        if num_btns > 0:
            self.container_layout.addSpacing(10)
            btn_header = QLabel("DIGITAL BUTTONS")
            btn_header.setStyleSheet("color: #777; font-weight: bold; font-size: 10px;")
            self.container_layout.addWidget(btn_header)

            btn_container = QWidget()
            btn_grid = QGridLayout(btn_container)
            btn_grid.setContentsMargins(0, 5, 0, 5)
            btn_grid.setSpacing(6)
            
            cols = 8 
            for i in range(num_btns):
                lbl = QLabel(str(i))
                lbl.setFixedSize(28, 28)
                lbl.setAlignment(Qt.AlignCenter)
                lbl.setStyleSheet("background: #222; color: #444; border-radius: 4px; font-weight: bold; font-size: 10px;")
                btn_grid.addWidget(lbl, i // cols, i % cols)
                self.btn_indicators.append(lbl)
            
            self.container_layout.addWidget(btn_container)

        self.container_layout.addSpacing(20)
        tuned_label = QLabel("--- TUNED / PROCESSED DATA (PENDING) ---")
        tuned_label.setAlignment(Qt.AlignCenter)
        tuned_label.setStyleSheet("color: #222; font-weight: bold; font-size: 11px;")
        self.container_layout.addWidget(tuned_label)

        self.container_layout.addStretch()

    def update_state(self, proc):
        # PERFORMANCE: Only update if the page is visible
        if not proc or not self.isVisible(): 
            return

        try:
            slots = proc.get_slots() 
            p1 = slots.get("1")
            
            is_connected = p1 and p1.get("conn", False)

            if is_connected:
                device_name = p1.get('name', 'HID Device')
                axes = p1.get('axes', [])
                btns = p1.get('btns', [])
                accel = p1.get('accel', [])
                gyro = p1.get('gyro', [])

                if self.current_device != device_name or len(self.axis_bars) != len(axes):
                    self.status_label.setText(f"PORT 1: {device_name.upper()}")
                    self.status_label.setStyleSheet("color: #00FF00; font-family: 'Consolas'; font-size: 20px; font-weight: bold;")
                    self.create_widgets(len(axes), len(btns), (len(accel) > 0))
                    self.current_device = device_name

                # Update 16-bit Analog Axis Bars
                for i, val in enumerate(axes):
                    if i < len(self.axis_bars):
                        self.axis_bars[i].setValue(int(val))

                # Update Accelerometer (Direct int mapping from scaled C++ value)
                for i, val in enumerate(accel):
                    if i < len(self.accel_bars):
                        self.accel_bars[i].setValue(int(val))

                # Update Gyroscope (Direct int mapping from scaled C++ value)
                for i, val in enumerate(gyro):
                    if i < len(self.gyro_bars):
                        self.gyro_bars[i].setValue(int(val))

                # Update Buttons
                for i, state in enumerate(btns):
                    if i < len(self.btn_indicators):
                        is_pressed = bool(state)
                        if is_pressed != self.btn_states[i]:
                            self.btn_states[i] = is_pressed
                            if is_pressed:
                                self.btn_indicators[i].setStyleSheet("background: #00FF00; color: #000; border-radius: 4px; font-weight: bold;")
                            else:
                                self.btn_indicators[i].setStyleSheet("background: #222; color: #444; border-radius: 4px; font-weight: bold;")
            else:
                if self.current_device is not None:
                    self.status_label.setText("PORT 1: DISCONNECTED")
                    self.status_label.setStyleSheet("color: #555; font-size: 20px; font-weight: bold;")
                    self.create_widgets(0, 0, False)
                    self.current_device = None
        except Exception:
            pass