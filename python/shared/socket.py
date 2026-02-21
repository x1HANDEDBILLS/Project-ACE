import socket
import os

class SocketClient:
    def __init__(self, path="/tmp/xace.sock"):
        self.path = path
        self.client = None

    def connect(self):
        if not os.path.exists(self.path): return False
        try:
            self.client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            self.client.connect(self.path)
            self.client.setblocking(False)
            return True
        except:
            self.client = None
            return False

    def receive(self):
        if not self.client: return None
        try:
            data = self.client.recv(65536)
            
            # If C++ app is closed, recv returns empty bytes
            if data == b'':
                self.disconnect()
                return None
                
            last_nl = data.rfind(b'\n')
            if last_nl == -1: return None
            
            start_nl = data.rfind(b'\n', 0, last_nl)
            return data[start_nl+1:last_nl].decode('utf-8', errors='ignore')
        except (BlockingIOError, socket.timeout):
            return None
        except Exception:
            self.disconnect()
            return None

    def disconnect(self):
        if self.client:
            try:
                self.client.close()
            except: pass
            self.client = None