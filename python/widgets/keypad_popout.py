from widgets.popout import TacticalPopout
from widgets.keypad import TacticalKeypad
from PySide6.QtCore import Signal, QTimer, Qt # Added QTimer and Qt
import theme

class KeypadPopout(TacticalPopout):
    value_confirmed = Signal(str)

    def __init__(self, parent, initial_val="", title="// TUNING_INPUT"):
        self.keypad = None 
        super().__init__(parent)
        
        self.base_w, self.base_h = 320, 480 
        self.total_w = self.base_w + (40 * 2) 
        self.total_h = self.base_h + (40 * 2)
        self.setFixedSize(self.total_w, self.total_h)
        
        self.keypad = TacticalKeypad(initial_val)
        self.set_content(self.keypad, title)
        
        self.keypad.submitted.connect(self._handle_submission)

    def _handle_submission(self, val):
        final_val = val if val != "" else "0"
        self.value_confirmed.emit(final_val)
        
        # 1. Start the visual glitch/slide out
        self.hide_hud()
        
        # 2. Fully destroy the widget after animation finishes (500ms)
        # This prevents memory leaks and "ghost" keypads
        QTimer.singleShot(500, self.close)

    def refresh_theme(self):
        super().refresh_theme()
        if hasattr(self, 'keypad') and self.keypad is not None:
            if hasattr(self.keypad, 'refresh_theme'):
                self.keypad.refresh_theme()