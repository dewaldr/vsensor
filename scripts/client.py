#!/usr/bin/python

import time
from socket import *

max_retries = 5
retried = 0
retry = True

while (retry):
    clientSocket = socket(AF_INET, SOCK_DGRAM)
    clientSocket.settimeout(3)

    # Encode message
    message = 'ArduinoNano,Q'
    addr = ("192.168.11.65", 5050)

    clientSocket.sendto(message, addr)

    try:
        data, server = clientSocket.recvfrom(1024)
        print '%s' % (data)
        retry = False

    except timeout:
        retried += 1
        if (retried < max_retries):
            time.sleep(1)
        else:
            print 'Error: request failed'
            retry = False


