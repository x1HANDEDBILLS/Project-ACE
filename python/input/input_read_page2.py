from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QGridLayout, 
                             QLabel, QProgressBar, QScrollArea)
from PySide6.QtCore import Qt

class InputReadPage2(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        # Main Layout setup
        self.main_layout = QVBoxLayout(self)
        self.main_layout.setContentsMargins(20, 20, 20, 20)

        # 1. Header Status Display
        self.status_label = QLabel("PORT 2: WAITING...")
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
        self.accel_label = None
        self.gyro_label = None
        self.current_device = None
        self.btn_states = []

    def create_widgets(self, num_axes, num_btns, has_motion):
        """Rebuilds UI with full hierarchy: RAW Group -> Component Sub-sections."""
        while self.container_layout.count():
            item = self.container_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        
        self.axis_bars = []
        self.btn_indicators = []
        self.btn_states = [False] * num_btns

        # --- GLOBAL GROUP: RAW DATA ---
        raw_group_header = QLabel("--- RAW HARDWARE INPUTS ---")
        raw_group_header.setAlignment(Qt.AlignCenter)
        raw_group_header.setStyleSheet("color: #FF5500; font-weight: bold; font-size: 12px; margin-top: 5px; margin-bottom: 5px;")
        self.container_layout.addWidget(raw_group_header)

        # --- SUB-SECTION: ANALOG AXES ---
        if num_axes > 0:
            ax_header = QLabel("ANALOG AXES")
            ax_header.setStyleSheet("color: #777; font-weight: bold; font-size: 10px; margin-top: 5px;")
            self.container_layout.addWidget(ax_header)

            for i in range(num_axes):
                bar = QProgressBar()
                bar.setRange(-32768, 32768) 
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
                    QProgressBar::chunk { background-color: #FF5500; }
                """)
                self.container_layout.addWidget(bar)
                self.axis_bars.append(bar)

        # --- SUB-SECTION: MOTION TELEMETRY ---
        if has_motion:
            self.container_layout.addSpacing(10)
            mot_header = QLabel("MOTION TELEMETRY")
            mot_header.setStyleSheet("color: #777; font-weight: bold; font-size: 10px;")
            self.container_layout.addWidget(mot_header)

            motion_box = QWidget()
            motion_box.setStyleSheet("background: #111; border: 1px solid #222; border-radius: 4px;")
            motion_vbox = QVBoxLayout(motion_box)
            
            label_style = "color: #00FFCC; font-family: 'Consolas', monospace; font-size: 12px; border: none;"
            self.accel_label = QLabel("ACCEL: X: 0.000 Y: 0.000 Z: 0.000")
            self.gyro_label = QLabel("GYRO:  P: 0.000 R: 0.000 Y: 0.000")
            
            self.accel_label.setStyleSheet(label_style)
            self.gyro_label.setStyleSheet(label_style)
            
            motion_vbox.addWidget(self.accel_label)
            motion_vbox.addWidget(self.gyro_label)
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

        # --- SEPARATOR FOR FUTURE SECTIONS ---
        self.container_layout.addSpacing(20)
        tuned_label = QLabel("--- TUNED / PROCESSED DATA (PENDING) ---")
        tuned_label.setAlignment(Qt.AlignCenter)
        tuned_label.setStyleSheet("color: #222; font-weight: bold; font-size: 11px;")
        self.container_layout.addWidget(tuned_label)

        self.container_layout.addStretch()

    def update_state(self, proc):
        if not proc: return
        try:
            slots = proc.get_slots() 
            p2 = slots.get("2") 
            
            is_connected = p2 and p2.get("conn", False)

            if is_connected:
                device_name = p2.get('name', 'HID Device')
                axes = p2.get('axes', [])
                btns = p2.get('btns', [])
                accel = p2.get('accel', [])
                gyro = p2.get('gyro', [])

                if self.current_device != device_name or len(self.axis_bars) != len(axes):
                    self.status_label.setText(f"PORT 2: {device_name.upper()}")
                    self.status_label.setStyleSheet("color: #FF5500; font-family: 'Consolas'; font-size: 20px; font-weight: bold;")
                    self.create_widgets(len(axes), len(btns), (len(accel) > 0))
                    self.current_device = device_name

                # Update Axis Bars
                for i, val in enumerate(axes):
                    if i < len(self.axis_bars):
                        self.axis_bars[i].setValue(int(val))

                # Update Motion Labels
                if self.accel_label and len(accel) >= 3:
                    self.accel_label.setText(f"ACCEL: X:{accel[0]:>7.3f} Y:{accel[1]:>7.3f} Z:{accel[2]:>7.3f}")
                if self.gyro_label and len(gyro) >= 3:
                    self.gyro_label.setText(f"GYRO:  P:{gyro[0]:>7.3f} R:{gyro[1]:>7.3f} Y:{gyro[2]:>7.3f}")

                # Update Buttons
                for i, state in enumerate(btns):
                    if i < len(self.btn_indicators):
                        is_pressed = (state == 1)
                        if is_pressed != self.btn_states[i]:
                            self.btn_states[i] = is_pressed
                            if is_pressed:
                                self.btn_indicators[i].setStyleSheet("background: #FF5500; color: #000; border-radius: 4px; font-weight: bold;")
                            else:
                                self.btn_indicators[i].setStyleSheet("background: #222; color: #444; border-radius: 4px; font-weight: bold;")
            else:
                if self.current_device is not None:
                    self.status_label.setText("PORT 2: DISCONNECTED")
                    self.status_label.setStyleSheet("color: #555; font-size: 20px; font-weight: bold;")
                    self.create_widgets(0, 0, False)
                    self.current_device = None
        except Exception:
            pass