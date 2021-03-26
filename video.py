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
buf = struct.pack("<HHHH", 0x534B, 1, 2, 0)
s.sendall(buf)

# Send Register message for FRAME message
# msgId = 3 is Register
# src = 2 is video node
# len = 4
# Payload
# 4 - msgId to register
# 1 - register
buf = struct.pack("<HHHHHH", 0x534B, 3, 2, 4, 4, 1)
s.sendall(buf)

time.sleep(2)

# Send LOGOUT message (ID, SrcNode, Len)
# msgId = 2 is login
# src = 2 is video node
# len = 0, for header only
buf = struct.pack("<HHHH", 0x534B, 2, 2, 0)
s.sendall(buf)

#data = s.recv(1024)

