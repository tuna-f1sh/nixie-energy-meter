#!/usr/bin/env python

import sys
import signal
import socket
import time
import struct

terminate = False

TCP_IP = '192.168.4.1'
TCP_PORT = 2121
BUFFER_SIZE = 8

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.setblocking(0)
s.connect((TCP_IP, TCP_PORT))

def signal_handling(signum,frame):           
    global terminate                         
    terminate = True    

signal.signal(signal.SIGINT,signal_handling)

# for _ in range(10):
while True:
    # msg = raw_input(">")
    s.send(bytearray([0xC0]))
    # data arrives as string with hex escapes
    data = s.recv(BUFFER_SIZE)
    # print "received data:", data
    # data = [elem.encode("hex") for elem in data] 
    # convert hex escapes to number
    raw = [struct.unpack('>B',elem)[0] for elem in data] 
    # make 16 bit from the two data bytes
    print raw[1] + (raw[2] << 7)
    time.sleep(1)
    if terminate:
        print "Ctrl-C detected"
        break

s.close()
print "Connection closed"
