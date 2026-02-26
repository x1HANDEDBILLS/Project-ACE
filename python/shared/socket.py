import socket
import os

class SocketClient:
    def __init__(self, path="/tmp/xace.sock"):
        self.path = path
        self.client = None

    def connect(self):
        """Initializes the connection to the C++ Engine."""
        if not os.path.exists(self.path): 
            return False
        try:
            self.client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            self.client.connect(self.path)
            # Non-blocking allows the UI to stay responsive even if no data is coming
            self.client.setblocking(False)
            return True
        except Exception:
            self.client = None
            return False

    def send(self, message):
        """Sends a command string to the C++ Engine (e.g., SET_SLOT|1|profile.json)"""
        if not self.client: 
            return False
        try:
            # Newline ensures the C++ side's poll_commands() can distinguish distinct messages
            self.client.sendall((message + "\n").encode('utf-8'))
            return True
        except Exception:
            self.disconnect()
            return False

    def receive(self):
        """Snaps the freshest JSON frame from the telemetry stream."""
        if not self.client: 
            return None
        try:
            # We read a large chunk (64KB) to clear out old data in the pipe.
            # This ensures we are always looking at the "Now".
            data = self.client.recv(65536)
            
            if not data:
                self.disconnect()
                return None
            
            # Find the very last newline in the entire buffer
            last_nl = data.rfind(b'\n')
            if last_nl == -1: 
                return None # No complete JSON object found in this chunk
            
            # Look for the newline immediately BEFORE the last one
            # This isolates the single most recent complete JSON string
            start_nl = data.rfind(b'\n', 0, last_nl)
            
            # Extract the frame:
            # If start_nl is -1, it means the buffer only had one line.
            final_frame = data[start_nl+1:last_nl].decode('utf-8', errors='ignore')
            
            return final_frame
            
        except (BlockingIOError, socket.timeout):
            # Normal: No data available at this exact microsecond
            return None
        except Exception:
            self.disconnect()
            return None

    def disconnect(self):
        """Safely closes the socket connection."""
        if self.client:
            try:
                self.client.close()
            except: 
                pass
            self.client = None