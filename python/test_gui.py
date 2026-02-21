import sys
import subprocess
import signal
import time
from PySide6.QtWidgets import QApplication, QMainWindow, QWidget, QFrame, QPushButton, QLabel
from PySide6.QtCore import Qt, QTimer

class AceUltraScaler(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ref_w, self.ref_h = 1024, 600
        
        self.central = QWidget()
        self.setCentralWidget(self.central)
        self.central.setStyleSheet("background: #000;")

        self.canvas = QFrame(self.central)
        self.sidebar = QFrame(self.canvas)
        self.btn1 = QPushButton("DASHBOARD", self.sidebar)
        self.btn2 = QPushButton("SETTINGS", self.sidebar)
        self.stats = QLabel(self.canvas)

        # Timer to check for hotplugs and resolution mismatches
        self.monitor_timer = QTimer()
        self.monitor_timer.timeout.connect(self.check_display_sync)
        self.monitor_timer.start(2000)

        self.last_w, self.last_h = 0, 0
        self.showFullScreen()
        self.refresh_layout()

    def check_display_sync(self):
        # Force the OS to acknowledge whatever is plugged in right now
        geo = QApplication.primaryScreen().geometry()
        if geo.width() != self.last_w or geo.height() != self.last_h:
            print(f"[RE-SYNC] Display changed to {geo.width()}x{geo.height()}")
            # This command fixes the "Black Screen" by forcing auto-detection
            subprocess.run("export DISPLAY=:0 && xrandr --auto", shell=True)
            time.sleep(0.5) 
            self.refresh_layout()

    def refresh_layout(self):
        try:
            output = subprocess.check_output("export DISPLAY=:0 && xrandr", shell=True).decode()
            is_btt = "1024x768" in output and "2560x1440" not in output
            
            geo = QApplication.primaryScreen().geometry()
            self.last_w, self.last_h = geo.width(), geo.height()

            use_w, use_h = (1024, 600) if is_btt else (self.last_w, self.last_h)
            
            # --- SCALING MATH ---
            rx = use_w / self.ref_w
            ry = use_h / self.ref_h
            
            # Scale Thicknesses
            border_thick = max(1, int(2 * rx))
            font_size = max(10, int(14 * rx))
            
            # --- DYNAMIC STYLES ---
            self.canvas.setStyleSheet(f"border: {border_thick}px solid #333; background: #050505;")
            self.sidebar.setStyleSheet(f"background: #111; border: {border_thick}px solid red; border-radius: {int(5*rx)}px;")
            
            btn_style = f"""
                QPushButton {{
                    background: #222; color: white; border: {max(1, int(1*rx))}px solid red; 
                    font-size: {font_size}px; font-family: 'Consolas'; font-weight: bold;
                }}
            """
            self.btn1.setStyleSheet(btn_style)
            self.btn2.setStyleSheet(btn_style)
            self.stats.setStyleSheet(f"color: lime; font-family: monospace; font-size: {font_size}px; border: none;")

            # --- GEOMETRY ---
            self.canvas.setGeometry(0, 0, use_w, use_h)
            
            side_w = int(160 * rx)
            self.sidebar.setGeometry(use_w - side_w - int(15*rx), int(15*ry), side_w, use_h - int(30*ry))

            btn_h = int(60 * ry)
            self.btn1.setGeometry(int(10*rx), int(20*ry), side_w - int(20*rx), btn_h)
            self.btn2.setGeometry(int(10*rx), int(90*ry), side_w - int(20*rx), btn_h)
            
            self.stats.setText(f"SCREEN: {self.last_w}x{self.last_h}\nFIX: {'BTT' if is_btt else 'NATIVE'}\nSCALE: {rx:.2f}x")
            self.stats.adjustSize()
            self.stats.move(int(20*rx), int(20*ry))

        except Exception as e:
            print(f"Scaling Error: {e}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    signal.signal(signal.SIGINT, lambda *args: QApplication.quit())
    win = AceUltraScaler()
    sys.exit(app.exec())
