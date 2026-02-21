import random
from PySide6.QtWidgets import QGraphicsEffect
from PySide6.QtCore import Qt, QObject, Signal, QTimer
import theme

class TacticalGlitchEffect(QGraphicsEffect):
    def __init__(self):
        super().__init__()
        self.intensity = 0

    def draw(self, painter):
        pixmap = self.sourcePixmap(Qt.DeviceCoordinates)
        if pixmap.isNull(): return
        w, h = pixmap.width(), pixmap.height()
        
        if self.intensity <= 0:
            painter.drawPixmap(0, 0, pixmap)
        else:
            num_slices = 10
            slice_h = max(1, h // num_slices)
            for i in range(num_slices):
                y_pos = i * slice_h
                x_off = random.randint(-int(self.intensity) * 2, int(self.intensity) * 2)
                painter.drawPixmap(x_off, y_pos, pixmap, 0, y_pos, w, slice_h)
            
            painter.setPen(theme.ACTIVE["primary"])
            for _ in range(int(self.intensity)):
                lx = random.randint(40, w - 70)
                ly = random.randint(40, h - 40)
                painter.drawRect(lx, ly, random.randint(10, 30), 1)

def get_telemetry_state(current_val, is_transitioning, is_dropping):
    if is_transitioning:
        return "// SIGNAL_INTERFERENCE", 0, False
    
    if is_dropping:
        target = float(random.randint(18, 30))
    else:
        target = random.uniform(92.0, 98.0)
    
    new_val = current_val + (target - current_val) * 0.12
    status_text = "// CONNECTION_STABLE" if not is_dropping else "// SIGNAL_DEGRADED"
    return status_text, new_val, is_dropping

# --- GLOBAL SYNC CONTROLLER ---

class TelemetryBroadcaster(QObject):
    """The central heart of the UI telemetry. Broadcasts to all UI elements."""
    # Signals out: (status_text, signal_value, is_dropping)
    updated = Signal(str, float, bool)

    def __init__(self):
        super().__init__()
        self.signal_val = 95.0
        self.is_dropping = False
        self.drop_hold_timer = 0
        self.timer = None

    def initialize(self):
        """Starts the pulse. Call this from the Main Thread after QApplication init."""
        if self.timer is None:
            self.timer = QTimer(self)
            self.timer.timeout.connect(self._tick)
            self.timer.start(200)

    def _tick(self):
        # Handle the logic of when a 'drop' occurs
        if self.is_dropping:
            self.drop_hold_timer += 1
            if self.drop_hold_timer > 25: # Hold the drop for ~5 seconds
                self.is_dropping = False
                self.drop_hold_timer = 0
        elif random.random() < 0.015: # 1.5% chance to start a drop
            self.is_dropping = True

        # Calculate the next state using your existing function
        msg, self.signal_val, self.is_dropping = get_telemetry_state(
            self.signal_val, False, self.is_dropping
        )
        
        # Broadcast this exact state to all connected listeners
        self.updated.emit(msg, self.signal_val, self.is_dropping)

# Single Global Instance
GLOBAL_TELEMETRY = TelemetryBroadcaster()