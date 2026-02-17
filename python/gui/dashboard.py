from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, 
                             QPushButton, QStackedWidget, QLabel, 
                             QProgressBar, QSlider, QGridLayout, QFrame)
from PySide6.QtCore import Qt
import theme
from widgets.color_picker import ColorEngineBox

class DashboardContent(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(15, 15, 15, 15)
        self.layout.setSpacing(10)

        # 1. Page Stack
        self.pages = QStackedWidget()
        self.setup_pages()

        # 2. Navigation Control Bar
        self.setup_nav_bar()

        self.layout.addWidget(self.pages)
        self.layout.addLayout(self.nav_layout)
        
        self.refresh_dashboard()

    def create_styled_frame(self):
        """Helper to create a tactical container for widgets."""
        frame = QFrame()
        frame.setObjectName("tacticalFrame")
        frame.setStyleSheet(f"""
            #tacticalFrame {{
                border: 1px solid {theme.ACTIVE['hex']};
                background: rgba(10, 10, 10, 0.8);
                border-radius: 2px;
            }}
        """)
        return frame

    def setup_pages(self):
        # We are building 8 unique pages now
        for i in range(1, 9):
            page = QWidget()
            page_layout = QVBoxLayout(page)
            
            # Header with ID tag
            header_layout = QHBoxLayout()
            header = QLabel(f"MODULE_0{i}_INIT")
            header.setObjectName("pageHeader")
            status_tag = QLabel("[ ONLINE ]")
            status_tag.setObjectName("statusTag")
            
            header_layout.addWidget(header)
            header_layout.addStretch()
            header_layout.addWidget(status_tag)
            page_layout.addLayout(header_layout)

            # Main content area
            container = self.create_styled_frame()
            container_layout = QVBoxLayout(container)
            
            self.inject_page_content(i, container_layout)
            
            page_layout.addWidget(container)
            self.pages.addWidget(page)

    def inject_page_content(self, index, layout):
        if index == 1: # POWER
            layout.addWidget(QLabel("PRIMARY BATTERY PACK"))
            p1 = QProgressBar(); p1.setValue(92); layout.addWidget(p1)
            layout.addWidget(QLabel("ESC THERMAL SENSORS"))
            p2 = QProgressBar(); p2.setValue(45); layout.addWidget(p2)
            
        elif index == 2: # NAVIGATION
            grid = QGridLayout()
            grid.addWidget(QLabel("ALTITUDE:"), 0, 0); grid.addWidget(QLabel("124m"), 0, 1)
            grid.addWidget(QLabel("VELOCITY:"), 1, 0); grid.addWidget(QLabel("42km/h"), 1, 1)
            grid.addWidget(QLabel("SATELLITES:"), 2, 0); grid.addWidget(QLabel("12 FIX"), 2, 1)
            layout.addLayout(grid)

        elif index == 3: # SIGNAL / RF
            layout.addWidget(QLabel("RX_LINK_STRENGTH"))
            s1 = QSlider(Qt.Horizontal); s1.setValue(80); layout.addWidget(s1)
            layout.addWidget(QLabel("TX_OUTPUT_DBM"))
            s2 = QSlider(Qt.Horizontal); s2.setValue(20); layout.addWidget(s2)

        elif index == 4: # COMMAND LOG
            log = QLabel("> Initializing Uplink...\n> Auth Success\n> Waiting for Telemetry...")
            log.setAlignment(Qt.AlignTop)
            layout.addWidget(log)

        elif index == 5: # RELAY SWITCHES
            grid = QGridLayout()
            for r in range(3):
                for c in range(2):
                    btn = QPushButton(f"SWITCH_{r}{c}")
                    btn.setCheckable(True)
                    btn.setMinimumHeight(50)
                    grid.addWidget(btn, r, c)
            layout.addLayout(grid)

        elif index == 6: # VIDEO FEED SETTINGS
            layout.addWidget(QLabel("BITRATE CONTROL"))
            s_v = QSlider(Qt.Horizontal); layout.addWidget(s_v)
            layout.addWidget(QPushButton("ENABLE OSD OVERLAY"))

        elif index == 7: # ADVANCED DIAGNOSTICS
            layout.addWidget(QLabel("I2C BUS STATUS: OK"))
            layout.addWidget(QLabel("CANBUS LOAD: 12%"))

        elif index == 8: # SETTINGS / COLOR
            layout.addWidget(QLabel("SYSTEM THEME CONFIGURATION"))
            self.picker = ColorEngineBox()
            layout.addWidget(self.picker)

    def setup_nav_bar(self):
        self.nav_layout = QHBoxLayout()
        self.prev_btn = QPushButton("<<")
        self.next_btn = QPushButton(">>")
        self.page_label = QLabel("PG. 1/8")
        
        self.nav_layout.addWidget(self.prev_btn)
        self.nav_layout.addStretch()
        self.nav_layout.addWidget(self.page_label)
        self.nav_layout.addStretch()
        self.nav_layout.addWidget(self.next_btn)
        
        self.prev_btn.clicked.connect(self.go_prev)
        self.next_btn.clicked.connect(self.go_next)

    def refresh_dashboard(self):
        color = theme.ACTIVE.get('hex', '#FF1E1E')
        
        # Apply the global QSS (Colors the progress bars/sliders)
        from PySide6.QtWidgets import QApplication
        QApplication.instance().setStyleSheet(theme.get_qss())

        # Update Frame Borders
        for frame in self.findChildren(QFrame, "tacticalFrame"):
            frame.setStyleSheet(f"border: 1px solid {color}; background: rgba(10,10,10,0.8);")

        # Update Nav Buttons & Labels
        nav_css = f"border: 1px solid {color}; color: {color}; padding: 8px; font-weight: bold;"
        self.prev_btn.setStyleSheet(nav_css)
        self.next_btn.setStyleSheet(nav_css)
        self.page_label.setStyleSheet(f"color: {color}; font-size: 14px; font-family: 'Consolas';")
        self.page_label.setText(f"PG. {self.pages.currentIndex() + 1}/8")

        # Update Page Headers
        for header in self.findChildren(QLabel, "pageHeader"):
            header.setStyleSheet(f"color: {color}; font-size: 18px; font-weight: bold;")
        for tag in self.findChildren(QLabel, "statusTag"):
            tag.setStyleSheet(f"color: {color}; font-size: 12px;")

    def go_next(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() + 1) % 8)
        self.refresh_dashboard()

    def go_prev(self):
        self.pages.setCurrentIndex((self.pages.currentIndex() - 1) % 8)
        self.refresh_dashboard()