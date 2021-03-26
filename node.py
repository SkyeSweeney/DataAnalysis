#!/usr/bin/python 

import socket
import struct
import time

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 5000         # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))

# Create message

# Send LOGIN message (ID, SrcNode, Len)
# msgId = 1 is login
# src = 2 is video node
# len = 0, for header only
buf = struct.pack("<HHH", 1, 2, 0)
s.sendall(buf)

time.sleep(5)

# Send LOGOUT message (ID, SrcNode, Len)
# msgId = 2 is login
# src = 2 is video node
# len = 0, for header only
buf = struct.pack("<HHH", 2, 2, 0)
s.sendall(buf)

#data = s.recv(1024)

