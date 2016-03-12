#!/usr/bin/env python

import sys
import signal
import socket
import time

terminate = False

TCP_IP = '192.168.1.206'
TCP_PORT = 2121
BUFFER_SIZE = 256

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.setblocking(0)
s.connect((TCP_IP, TCP_PORT))

def signal_handling(signum,frame):           
    global terminate                         
    terminate = True    

signal.signal(signal.SIGINT,signal_handling)

# for _ in range(10):
while True:
    msg = raw_input(">")
    s.send(msg)
    data = s.recv(BUFFER_SIZE)
    print "received data:", data
    if terminate:
        print "Ctrl-C detected"
        break

s.close()
print "Connection closed"
