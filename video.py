#!/usr/bin/python 

import socket
import struct
import time

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 5000         # The port used by the server

src = 2  # Video
som = 0x534b

MSGID_UNK      = 0
MSGID_LOGIN    = 1
MSGID_LOGOUT   = 2
MSGID_EXIT     = 3
MSGID_REGISTER = 4
MSGID_FRAME    = 5
MSGID_LOG      = 6
MSGID_MAX      = 7

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))

# Create message

# Send LOGIN message (ID, SrcNode, Len)
# msgId = 1 is login
# src = 2 is video node
# len = 0, for header only
buf = struct.pack("<HHHH", som, MSGID_LOGIN, src, 0)
s.sendall(buf)

# Register for FRAME message
# msgId = 3 is Register
# src = 2 is video node
# len = 4
# Payload
# 4 - msgId to register
# 1 - register
buf = struct.pack("<HHHHHH", som, MSGID_REGISTER, src, 4, MSGID_FRAME, 1)
s.sendall(buf)

# Wait as we receive messages
while True:

    hdr = s.recv(8)
    if len(hdr) == 0: break

    som, msgId, source, length = struct.unpack("HHHH", hdr)
    if (som != 0x534B):
        print("Bad SOM")
        continue
    else:
        body = s.recv(length)
        if len(body) == 0: break
    #
    print("Got message %d frm %d" % (msgId, source))
#


# Send LOGOUT message (ID, SrcNode, Len)
# msgId = 2 is login
# src = 2 is video node
# len = 0, for header only
buf = struct.pack("<HHHH", som, MSGID_LOGOUT, src, 0)
s.sendall(buf)

#data = s.recv(1024)

