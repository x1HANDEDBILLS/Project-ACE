from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit, QProgressBar, QFrame
from PySide6.QtCore import Qt
import theme
from shared.socket import GLOBAL_SOCKET
from widgets.keypad_popout import KeypadPopout

class AxisTuneRow(QFrame):
    """A single row representing one of the 12 axes with dual telemetry bars and live values."""
    def __init__(self, axis_id, parent=None):
        super().__init__(parent)
        self.axis_id = axis_id
        self.setObjectName("AxisRow")
        self.setStyleSheet("background: transparent; border: none;")
        
        self.layout = QHBoxLayout(self)
        self.layout.setContentsMargins(10, 2, 10, 2)
        self.layout.setSpacing(12)

        # 1. AXIS IDENTIFIER
        self.label = QLabel(f"AXIS {axis_id:02d}")
        self.label.setFixedWidth(65)

        # 2. RAW INPUT VISUALIZER (Top Label + Bar)
        self.raw_container = QVBoxLayout()
        self.raw_header = QHBoxLayout()
        self.raw_title = QLabel("RAW")
        self.raw_val_text = QLabel("0") 
        self.raw_val_text.setAlignment(Qt.AlignRight)
        self.raw_header.addWidget(self.raw_title)
        self.raw_header.addWidget(self.raw_val_text)
        
        self.raw_bar = QProgressBar()
        self.raw_bar.setRange(-32768, 32767)
        self.raw_bar.setTextVisible(False)
        self.raw_bar.setFixedHeight(12) 
        
        self.raw_container.setSpacing(1)
        self.raw_container.addLayout(self.raw_header)
        self.raw_container.addWidget(self.raw_bar)

        # 3. DEADZONE TUNING INPUTS
        self.in_lbl = QLabel("IN %:")
        self.out_lbl = QLabel("OUT %:")
        
        self.inner_dz = QLineEdit("5") 
        self.outer_dz = QLineEdit("100") 
        
        for box in [self.inner_dz, self.outer_dz]:
            box.setFixedWidth(45) 
            box.setFixedHeight(26) 
            box.setAlignment(Qt.AlignCenter)
            box.setReadOnly(True)
            box.mousePressEvent = lambda event, b=box: self.open_keypad(b)

        # 4. TUNED OUTPUT VISUALIZER (Top Label + Bar)
        self.tuned_container = QVBoxLayout()
        self.tuned_header = QHBoxLayout()
        self.tuned_title = QLabel("TUNED")
        self.tuned_val_text = QLabel("0")
        self.tuned_val_text.setAlignment(Qt.AlignRight)
        self.tuned_header.addWidget(self.tuned_title)
        self.tuned_header.addWidget(self.tuned_val_text)
        
        self.tuned_bar = QProgressBar()
        self.tuned_bar.setRange(-32768, 32767)
        self.tuned_bar.setTextVisible(False)
        self.tuned_bar.setFixedHeight(12)
        
        self.tuned_container.setSpacing(1)
        self.tuned_container.addLayout(self.tuned_header)
        self.tuned_container.addWidget(self.tuned_bar)

        # Assemble Row
        self.layout.addWidget(self.label)
        self.layout.addLayout(self.raw_container, 2)
        self.layout.addWidget(self.in_lbl)
        self.layout.addWidget(self.inner_dz)
        self.layout.addWidget(self.out_lbl)
        self.layout.addWidget(self.outer_dz)
        self.layout.addLayout(self.tuned_container, 2)

    def open_keypad(self, target_box):
        """Spawns the tactical keypad popout."""
        main_window = self.window() 
        self.pop = KeypadPopout(main_window, target_box.text(), f"// AXIS_{self.axis_id}_CFG")
        self.pop.value_confirmed.connect(lambda val: self.apply_value(target_box, val))
        self.pop.show_hud()

    def apply_value(self, target_box, val):
        """Updates UI and triggers the socket send."""
        target_box.setText(val)
        self.send_update_to_cpp()

    def send_update_to_cpp(self):
        """Pushes data via GLOBAL_SOCKET. Format: SET_DZ|TYPE|ID|IN|OUT"""
        try:
            in_val = int(self.inner_dz.text()) if self.inner_dz.text() else 0
            out_val = int(self.outer_dz.text()) if self.outer_dz.text() else 100
            
            in_val = max(0, min(in_val, 100))
            out_val = max(0, min(out_val, 100))

            # \n is vital for the C++ side's getline/sscanf parser
            cmd = f"SET_DZ|1|{self.axis_id}|{in_val}|{out_val}\n"
            GLOBAL_SOCKET.send(cmd)
        except Exception as e:
            print(f"SOCKET ERROR: {e}")

    def update_telemetry(self, raw_val, tuned_val):
        """Updates bars and handles 'Deadzone Active' visual feedback."""
        r = int(raw_val)
        t = int(tuned_val)
        
        self.raw_bar.setValue(r)
        self.raw_val_text.setText(str(r))
        
        self.tuned_bar.setValue(t)
        self.tuned_val_text.setText(str(t))
        
        # If hardware is moving (Raw > 100) but output is 0, highlight orange (Deadzone working)
        if t == 0 and abs(r) > 100:
            self.raw_val_text.setStyleSheet("color: #FFAA00; font-weight: bold; font-family: 'Consolas'; font-size: 10px;")
        else:
            self.raw_val_text.setStyleSheet("color: #888888; font-family: 'Consolas'; font-size: 10px;")
            
        self.tuned_val_text.setStyleSheet(f"color: {theme.ACTIVE['hex']}; font-weight: bold; font-family: 'Consolas'; font-size: 10px;")

    def refresh_theme(self):
        h = theme.ACTIVE['hex']
        accent = f"color: {h}; font-family: 'Consolas'; font-weight: bold; font-size: 12px;"
        lbl_style = "font-size: 9px; color: #888888; font-weight: bold; font-family: 'Consolas';"
        
        input_style = f"""
            QLineEdit {{
                background: rgba(0, 0, 0, 0.6);
                border: 1px solid {h}44;
                color: white;
                font-family: 'Consolas';
                font-size: 13px;
                border-radius: 4px;
            }}
        """
        bar_style = f"""
            QProgressBar {{
                background: rgba(255, 255, 255, 0.05);
                border: none;
                border-radius: 2px;
            }}
            QProgressBar::chunk {{
                background: {h};
            }}
        """
        
        self.label.setStyleSheet(accent)
        self.raw_title.setStyleSheet(lbl_style)
        self.tuned_title.setStyleSheet(lbl_style)
        self.in_lbl.setStyleSheet(lbl_style)
        self.out_lbl.setStyleSheet(lbl_style)
        self.inner_dz.setStyleSheet(input_style)
        self.outer_dz.setStyleSheet(input_style)
        self.raw_bar.setStyleSheet(bar_style)
        self.tuned_bar.setStyleSheet(bar_style)

class TunePage1(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(10, 5, 10, 5)
        self.layout.setSpacing(4)

        self.rows = []
        for i in range(12):
            row = AxisTuneRow(i)
            self.rows.append(row)
            self.layout.addWidget(row)
            
        self.refresh_theme()

    def update_state(self, state_dict):
        raw_data = state_dict.get('raw_axes', [0]*12)
        tuned_data = state_dict.get('tuned_axes', [0]*12)
        
        for i, row in enumerate(self.rows):
            if i < len(raw_data) and i < len(tuned_data):
                row.update_telemetry(raw_data[i], tuned_data[i])

    def refresh_theme(self):
        for row in self.rows:
            row.refresh_theme()