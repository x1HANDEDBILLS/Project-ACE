import time
import json
import os

class Logic:
    def __init__(self):
        # --- CORE DATA STORAGE ---
        self.data = {"slots": {}} 
        self.state = {
            "raw_axes": [0] * 12,
            "tuned_axes": [0] * 12,
            "btns": [0] * 32  # Expanded to match your C++ 32-bit button array
        }
        
        # --- SLOT MANAGEMENT ---
        # Toggle this to "4" if you are testing the secondary controller
        self.active_slot = "1" 
        
        # --- TELEMETRY & HEARTBEAT ---
        self.heartbeat = 0
        self.late_count = 0
        self.last_hb = 0
        self.last_packet_time = time.perf_counter()
        self.current_hz = 0
        
        # --- COMMAND PIPELINE ---
        self.command_queue = []
        
        # --- PERFORMANCE TRACKING ---
        self.session_start_time = 0
        self.session_start_hb = 0
        self.session_start_late = 0
        self.window_start_time = time.perf_counter()
        self.window_start_hb = 0
        
        # --- STABILITY CONTROLS ---
        self.settling_counter = 0
        self.SETTLE_THRESHOLD = 100 
        
        # --- LOCAL FALLBACK TUNING ---
        self.EXPO = 0.4
        self.DEADBAND = 0.05 

    # --- IPC COMMAND MANAGEMENT ---

    def push_command(self, cmd):
        """Adds a command to the queue to be sent to C++."""
        self.command_queue.append(cmd)

    def pop_command(self):
        """Retrieves the next command for the LogicWorker to send via socket."""
        return self.command_queue.pop(0) if self.command_queue else None

    def get_sync_commands(self):
        """Builds sync list from settings.json to configure C++ slots."""
        cmds = []
        base_dir = os.path.dirname(os.path.abspath(__file__))
        settings_path = os.path.join(base_dir, "..", "settings.json")
        
        if os.path.exists(settings_path):
            try:
                with open(settings_path, 'r') as f:
                    settings = json.load(f)
                    active_slots = settings.get("active_slots", {})
                    for slot, profile in active_slots.items():
                        if profile and profile != "Empty":
                            cmds.append(f"SET_SLOT|{slot}|{profile}")
            except Exception as e:
                print(f"[Logic] Sync error: {e}")
        return cmds

    # --- TELEMETRY PROCESSING ---

    def update(self, raw_string):
        """
        Parses the JSON stream from C++ and updates internal state.
        Handles high-speed packet validation and slot extraction.
        """
        if not raw_string: 
            return
            
        try:
            # Clean up potential double commas (common in high-speed UDP/Unix streams)
            clean_json = raw_string.replace(",,", ",")
            packet = json.loads(clean_json)
        except json.JSONDecodeError: 
            return

        now = time.perf_counter()
        new_hb = packet.get("hb", 0)
        new_late = packet.get("late", 0)

        # 1. Reset/Desync Detection
        # If HB jumps backwards or gaps significantly, we reset performance tracking.
        if new_hb < self.last_hb or (new_hb - self.last_hb) > 5000 or self.last_hb == 0:
            self.settling_counter = self.SETTLE_THRESHOLD
            self.session_start_hb = 0
            self.session_start_time = 0
            self.current_hz = 0
            self.last_hb = new_hb
            return

        # 2. Settling Period
        # Ignores first 100 frames to ensure IMU buffers are clear.
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

        # 3. Rolling Hz Calculation (Updates every 100ms)
        win_duration = now - self.window_start_time
        if win_duration >= 0.1:
            hb_diff = new_hb - self.window_start_hb
            self.current_hz = hb_diff / win_duration
            self.window_start_time = now
            self.window_start_hb = new_hb

        # 4. DATA EXTRACTION FOR UI & RC CONTROL
        slots = packet.get("slots", {})
        
        if self.active_slot in slots:
            device = slots[self.active_slot]
            if device.get("conn"):
                self.state["raw_axes"] = device.get("raw", [0] * 12)
                self.state["tuned_axes"] = device.get("tuned", [0] * 12)
                self.state["btns"] = device.get("btns", [0] * 32)
                # Attach IMU data if present
                self.state["accel"] = device.get("accel", [0, 0, 0])
                self.state["gyro"] = device.get("gyro", [0, 0, 0])
            else:
                # Failsafe: Zero out axes if controller is disconnected
                self.state["raw_axes"] = [0] * 12
                self.state["tuned_axes"] = [0] * 12
        
        # 5. GENERAL UPDATES
        self.last_hb = new_hb
        self.heartbeat = new_hb
        self.late_count = new_late
        self.data = packet

    # --- UI & DIAGNOSTIC HELPERS ---

    def get_accuracy(self):
        """Calculates how many packets we actually received vs. expected."""
        TARGET_HZ = 2000.0
        if self.session_start_time == 0: return 0.0
        total_elapsed = time.perf_counter() - self.session_start_time
        if total_elapsed < 0.05: return 100.0
        expected_hb_count = total_elapsed * TARGET_HZ
        actual_hb_count = self.heartbeat - self.session_start_hb
        return max(0.0, min(100.0, (actual_hb_count / expected_hb_count) * 100.0))

    def get_jitter(self):
        """Calculates the percentage of 'late' frames flagged by the C++ engine."""
        total_ticks = self.heartbeat - self.session_start_hb
        if total_ticks <= 0: return 0.0
        lates_in_session = self.late_count - self.session_start_late
        return max(0.0, min(100.0, (lates_in_session / total_ticks) * 100.0))

    def get_slots(self):
        return self.data.get("slots", {})