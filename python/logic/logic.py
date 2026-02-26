import time
import json
import os

class Logic:
    def __init__(self):
        self.data = {"slots": {}} 
        self.heartbeat = 0
        self.late_count = 0
        self.last_hb = 0
        self.last_packet_time = time.perf_counter()
        self.current_hz = 0
        
        self.command_queue = []
        
        # Performance Tracking
        self.session_start_time = 0
        self.session_start_hb = 0
        self.session_start_late = 0
        self.window_start_time = time.perf_counter()
        self.window_start_hb = 0
        
        self.settling_counter = 0
        self.SETTLE_THRESHOLD = 100 
        
        # --- TUNED FOR 16-BIT ---
        self.EXPO = 0.4
        self.DEADBAND = 0.05  # 5% deadzone

    def push_command(self, cmd):
        self.command_queue.append(cmd)

    def pop_command(self):
        return self.command_queue.pop(0) if self.command_queue else None

    def get_sync_commands(self):
        """Builds sync list from settings.json to ensure C++ matches the UI on startup."""
        cmds = []
        settings_path = "settings.json" # Adjust to your absolute path
        if os.path.exists(settings_path):
            try:
                with open(settings_path, 'r') as f:
                    settings = json.load(f)
                    for slot, profile in settings.get("active_slots", {}).items():
                        if profile and profile != "Empty":
                            cmds.append(f"SET_SLOT|{slot}|{profile}")
            except Exception as e:
                print(f"[Logic] Sync error: {e}")
        return cmds

    # --- UPDATED STICK LOGIC ---
    def process_stick(self, raw_val):
        """
        Handles 16-bit input (-32768 to 32767).
        Normalizes to -1.0 to 1.0 range before applying Expo curves.
        """
        # 1. Normalize to -1.0 to 1.0
        # Dividing by 32768.0 ensures our math is float-perfect
        x = raw_val / 32768.0
        
        # 2. Apply Deadband
        if abs(x) < self.DEADBAND: 
            return 0.0
        
        # 3. Apply Expo Curve (The 'PlayStation' Feel)
        # Formula: (Expo * x^3) + ((1 - Expo) * x)
        return (self.EXPO * (x**3)) + ((1 - self.EXPO) * x)

    def update(self, raw_string):
        if not raw_string: return
        try:
            packet = json.loads(raw_string)
        except: return

        now = time.perf_counter()
        new_hb = packet.get("hb", 0)
        new_late = packet.get("late", 0)

        # 1. Reset/Desync Detection
        if new_hb < self.last_hb or (new_hb - self.last_hb) > 5000 or self.last_hb == 0:
            self.settling_counter = self.SETTLE_THRESHOLD
            self.session_start_hb = 0
            self.session_start_time = 0
            self.current_hz = 0
            self.last_hb = new_hb
            return

        # 2. Settling Period (Waiting for high-speed stream to stabilize)
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

        # 3. Frequency Window (Rolling Hz average)
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
        """Calculates late-frame percentage (lower is better)."""
        total_ticks = self.heartbeat - self.session_start_hb
        if total_ticks <= 0: return 0.0
        lates_in_session = self.late_count - self.session_start_late
        return max(0.0, min(100.0, (lates_in_session / total_ticks) * 100.0))

    def get_slots(self):
        return self.data.get("slots", {})