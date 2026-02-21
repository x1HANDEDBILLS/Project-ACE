import sys, os, time, gc, signal, psutil
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QThread, QTimer

def optimize_system():
    try:
        p = psutil.Process()
        p.cpu_affinity([2, 3]) 
        p.nice(-20)
        gc.set_threshold(100000, 10, 10) 
    except: pass

sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
from shared.socket import SocketClient
from logic.logic import Logic
from dash.dashboard import Dashboard

class LogicWorker(QThread):
    def __init__(self, comm, proc):
        super().__init__()
        self.comm, self.proc = comm, proc
        self.running = True

    def run(self):
        self.setPriority(QThread.TimeCriticalPriority)
        while self.running:
            if not self.comm.client:
                if self.comm.connect():
                    print("[ACE] Socket Connected.")
                else:
                    self.proc.current_hz = 0
                    self.proc.heartbeat = 0
                    time.sleep(0.5)
                    continue
            
            raw = self.comm.receive()
            if raw:
                self.proc.update(raw)
            else:
                # If receive returned None, it might be a disconnect
                time.sleep(0.0001)

def run():
    optimize_system()
    app = QApplication(sys.argv)
    
    comm = SocketClient("/tmp/xace.sock")
    proc = Logic()
    view = Dashboard()
    
    worker = LogicWorker(comm, proc)
    worker.start()

    ui_timer = QTimer()
    ui_timer.timeout.connect(lambda: view.show(proc))
    ui_timer.start(16)

    def handle_exit(*args):
        print("\n[ACE] Shutting down...")
        worker.running = False
        worker.wait()
        app.quit()

    signal.signal(signal.SIGINT, handle_exit)
    signal.signal(signal.SIGTERM, handle_exit)

    sys.exit(app.exec())

if __name__ == "__main__":
    run()