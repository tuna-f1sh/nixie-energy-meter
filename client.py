#!/usr/bin/env python

import socket
import time


TCP_IP = '192.168.0.2'
TCP_PORT = 80
BUFFER_SIZE = 256

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.setblocking(0)
s.connect((TCP_IP, TCP_PORT))
for _ in range(10):
    msg = raw_input(">")
    s.send(msg)
    data = s.recv(BUFFER_SIZE)
    print "received data:", data

s.close()

