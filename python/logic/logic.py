import time
import json

class Logic:
    def __init__(self):
        self.data = {}
        self.heartbeat = 0
        self.late_count = 0
        self.last_hb = 0
        self.last_packet_time = time.perf_counter()
        self.current_hz = 0
        
        # Accuracy Tracking
        self.session_start_time = 0
        self.session_start_hb = 0
        self.session_start_late = 0
        
        # Speed Tracking
        self.window_start_time = time.perf_counter()
        self.window_start_hb = 0
        
        self.settling_counter = 0
        self.SETTLE_THRESHOLD = 100 
        
        self.EXPO = 0.4
        self.DEADBAND = 0.05

    def process_stick(self, raw_val):
        """ FULL ORIGINAL STICK LOGIC: Handles 128-bit scaling and expo curves """
        x = raw_val / 128.0
        if abs(x) < self.DEADBAND: return 0.0
        return (self.EXPO * (x**3)) + ((1 - self.EXPO) * x)

    def update(self, raw_string):
        if not raw_string: return
        try:
            packet = json.loads(raw_string)
        except: return

        now = time.perf_counter()
        new_hb = packet.get("hb", 0)
        new_late = packet.get("late", 0)

        # 1. Reset Logic
        if new_hb < self.last_hb or (new_hb - self.last_hb) > 5000 or self.last_hb == 0:
            self.settling_counter = self.SETTLE_THRESHOLD
            self.session_start_hb = 0
            self.session_start_time = 0
            self.session_start_late = 0
            self.current_hz = 0
            self.last_hb = new_hb
            return

        # 2. Settling Period Logic
        if self.settling_counter > 0:
            self.settling_counter -= 1
            if self.settling_counter == 0:
                self.session_start_hb = new_hb
                self.session_start_time = now
                self.session_start_late = new_late
                self.window_start_hb = new_hb
                self.window_start_time = now
            self.last_hb = new_hb
            return

        # 3. Frequency Window calculation
        win_duration = now - self.window_start_time
        if win_duration >= 0.1:
            hb_diff = new_hb - self.window_start_hb
            self.current_hz = hb_diff / win_duration
            self.window_start_time = now
            self.window_start_hb = new_hb

        self.last_hb = new_hb
        self.heartbeat = new_hb
        self.late_count = new_late
        self.data = packet

    def get_accuracy(self):
        TARGET_HZ = 2000.0
        if self.session_start_time == 0: return 0.0
        total_elapsed = time.perf_counter() - self.session_start_time
        if total_elapsed < 0.05: return 100.0
        expected_hb_count = total_elapsed * TARGET_HZ
        actual_hb_count = self.heartbeat - self.session_start_hb
        return max(0.0, min(100.0, (actual_hb_count / expected_hb_count) * 100.0))

    def get_jitter(self):
        total_ticks = self.heartbeat - self.session_start_hb
        if total_ticks <= 0: return 0.0
        lates_in_session = self.late_count - self.session_start_late
        return max(0.0, min(100.0, (lates_in_session / total_ticks) * 100.0))

    def get_slots(self):
        return self.data.get("slots", [])