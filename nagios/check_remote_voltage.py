#!/usr/bin/python
import sys
import time
from socket import *

# Nagios exit codes:
# 0 ->  OK
# 1 ->  WARNING
# 2 ->  CRITICAL
# 3 ->  UNKNOWN

# Global options container
class Opts(object):
    pass

opts = Opts()

def main():
    import csv

    MAX_RETRIES = 5
    retried = 0

    while (True):
        clientSocket = socket(AF_INET, SOCK_DGRAM)
        clientSocket.settimeout(3)

        # Encode message
        request = "%s" % opts.auth
        addr = (opts.ip, opts.port)

        clientSocket.sendto(request, addr)

        try:
            response, server = clientSocket.recvfrom(1024)

            # TODO: evaluate response
            # print "%s" % (response)
            reader = csv.reader([response], skipinitialspace=True)
            for r in reader:
                print r
            sys.exit(0)

        except timeout:
            retried += 1
            if (retried < MAX_RETRIES):
                time.sleep(1)
            else:
                print 'Error: request failed'
                sys.exit(3)

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    #parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")
    parser.add_argument("-i", "--ip", type=str, required=True, help="Sensor IP address")
    parser.add_argument("-p", "--port", type=int, default=5050, help="Sensor UDP port")
    parser.add_argument("-a", "--auth", type=str, default="ArduinoNano", help="Sensor authentication passphrase")
    parser.add_argument("-c", "--critical", type=float, default=23.2, help="Critical voltage level")
    parser.add_argument("-w", "--warning", type=float, default=24.0, help="Warning voltage level")

    parser.parse_args(namespace=opts)

    main()
