import sys, os, time, gc, signal, psutil
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QThread, QTimer

# 1. SYSTEM OPTIMIZATION
def optimize_system():
    """Configures the Pi 4 for high-performance low-latency execution."""
    try:
        p = psutil.Process()
        # Pin process to isolated cores (2 and 3) to avoid OS interrupts
        p.cpu_affinity([2, 3]) 
        # Set to highest priority
        p.nice(-20)
        # Increase GC threshold to reduce collection frequency during runtime
        gc.set_threshold(100000, 10, 10) 
    except Exception as e:
        print(f"[WARN] Optimization failed: {e}")

# Ensure local imports work correctly
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))

# 2. HUD & LOGIC IMPORTS
from shared.socket import SocketClient
from logic.logic import Logic
from dash.dashboard import Dashboard
from effects.animations import GLOBAL_TELEMETRY

class LogicWorker(QThread):
    """
    Dedicated thread for high-speed socket polling.
    Handles both receiving telemetry and sending commands to the C++ engine.
    """
    def __init__(self, comm, proc):
        super().__init__()
        self.comm, self.proc = comm, proc
        self.running = True

    def run(self):
        # Request maximum scheduling priority from the OS
        self.setPriority(QThread.TimeCriticalPriority)
        
        while self.running:
            # Handle Connection
            if not self.comm.client:
                if self.comm.connect():
                    print("[ACE] Socket Connected. Synchronizing profiles...")
                    # Immediately sync C++ with current Python settings
                    for cmd in self.proc.get_sync_commands():
                        self.comm.send(cmd)
                else:
                    self.proc.current_hz = 0
                    self.proc.heartbeat = 0
                    time.sleep(0.5)
                    continue
            
            # --- TWO-WAY IPC INTERACTION ---

            # A. OUTBOUND: Check if Logic has commands (SET_SLOT, etc.) to send to C++
            cmd = self.proc.pop_command()
            if cmd:
                self.comm.send(cmd)

            # B. INBOUND: Receive telemetry from the C++ socket
            raw = self.comm.receive()
            if raw:
                self.proc.update(raw)
            else:
                # Keep latency ultra-low without pegged CPU
                time.sleep(0.0001)

def run():
    # Execute system tweaks before starting Qt
    optimize_system()
    
    app = QApplication(sys.argv)
    
    # CRITICAL: Initialize the Global Telemetry pulse in the Main Thread.
    GLOBAL_TELEMETRY.initialize()

    # Setup core components
    comm = SocketClient("/tmp/xace.sock")
    proc = Logic()
    
    # The Dashboard handles the UI and user input
    view = Dashboard()
    
    # Start the high-priority logic thread
    worker = LogicWorker(comm, proc)
    worker.start()

    # Main UI Refresh Timer (~60 FPS)
    ui_timer = QTimer()
    ui_timer.timeout.connect(lambda: view.show_update(proc))
    ui_timer.start(16)

    # 3. CLEAN SHUTDOWN HANDLER
    def handle_exit(*args):
        print("\n[ACE] Terminating sequence initiated...")
        worker.running = False
        worker.wait() 
        app.quit()

    # Capture OS signals
    signal.signal(signal.SIGINT, handle_exit)
    signal.signal(signal.SIGTERM, handle_exit)

    # Launch the Qt Event Loop
    sys.exit(app.exec())

if __name__ == "__main__":
    run()