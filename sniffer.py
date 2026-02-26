import socket
import os

# Use the file path we found in your /tmp folder
SOCKET_PATH = "/tmp/xace.sock"

# AF_UNIX = Unix File Socket | SOCK_STREAM = Connection-based (as seen in your C++ log)
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

try:
    print(f"Connecting to {SOCKET_PATH}...")
    sock.connect(SOCKET_PATH)
    print("SUCCESS: Data incoming...")
    
    while True:
        data = sock.recv(2048) # Similar to recvfrom
        if not data:
            break
        print(f"RAW JSON: {data.decode('utf-8', errors='ignore')}")

except Exception as e:
    print(f"ERROR: {e}")
    print("NOTE: Close the main Python GUI before running this, or they will fight for the connection.")
finally:
    sock.close()