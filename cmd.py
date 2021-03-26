#!/usr/bin/python 

########################################################################
# Command interface for Data Analysis
########################################################################

import socket
import struct
import time

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 5000         # The port used by the server
src = 1
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


# Send LOGIN message (ID, SrcNode, Len)
# msgId = 1 is login
# src = 1 is cmd node
# len = 0, for header only
buf = struct.pack("<HHHH", som, MSGID_LOGIN, src, 0)
s.sendall(buf)


# Register for LOG message
# msgId = 3 is Register
# src = 1 is video node
# len = 4
# Payload
# 5 - msgId to register
# 1 - register
buf = struct.pack("<HHHHHH", som, MSGID_REGISTER, src, 4, MSGID_LOG, 1)
s.sendall(buf)

# Spawn off thread to receive messages

while True:

    # Get user command
    cmd = raw_input("Cmd>")

    # Parse command
    toks = cmd.split()

    # Switch based on command
    if ("frame" in toks[0]):

        # Compose message
        # Send Register message for FRAME message
        # msgId = 4 is Frame
        # src = 1 is cmd node
        # len = 12
        # Payload
        # frm - 
        # sec - 
        # nsec -
        frm = 10
        sec = 1234332
        nsec = 1234553
        buf = struct.pack("<HHHHIII", som, MSGID_FRAME, src, 12, frm, sec, nsec)

        # Send command
        s.sendall(buf)

    elif ("exit" in toks[0]):
        break
    #
   
#


# Send LOGOUT message (ID, SrcNode, Len)
# msgId = 2 is login
# src = 1 is cdmd node
# len = 0, for header only
buf = struct.pack("<HHHH", som, MSGID_LOGOUT, src, 0)
s.sendall(buf)




