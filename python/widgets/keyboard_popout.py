from widgets.popout import TacticalPopout
from widgets.keyboard import TacticalKeyboard
from PySide6.QtCore import Signal

class KeyboardPopout(TacticalPopout):
    # Signal that passes the final string back to the row
    value_confirmed = Signal(str)

    def __init__(self, parent, initial_val="", title="// INPUT_TERMINAL"):
        super().__init__(parent)
        # Expand size for full QWERTY
        self.base_w, self.base_h = 600, 480 
        self.setFixedSize(self.total_w, self.total_h)
        
        self.kb_widget = TacticalKeyboard(initial_val)
        self.set_content(self.kb_widget, title)
        
        # Wire up internal keyboard to the popout closure
        self.kb_widget.submitted.connect(self._on_submit)

    def _on_submit(self, val):
        self.value_confirmed.emit(val)
        self.hide_hud()