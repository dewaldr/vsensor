#!/usr/bin/python
import socket
import sys

MAX_MSG_SIZE = 128

# Nagios exit codes
NAGIOS_OK       = 0
NAGIOS_WARNING  = 1
NAGIOS_CRITICAL = 2
NAGIOS_UNKNOWN  = 3

# Global options container
class Opts(object):
    pass

opts = Opts()

def isfloat(value):
    try:
        float(value)
        return True
    except:
        return False

def main():

    exitCode = NAGIOS_UNKNOWN
    sock = socket.create_connection((opts.ip, opts.port))

    try:
        request = '%s,%s.' % (opts.auth, opts.command)
        sock.sendall(request)

        response = sock.recv(MAX_MSG_SIZE)

        if ((len(response) > 4) and response.endswith('.')):

            # Drop trailing '.' and split
            data = (response[:-1]).split(',')
            voltage = 0.0

            if (isfloat(data[2])):
                voltage = float(data[2])

            if ((voltage <= opts.warning) and (voltage > opts.critical)):
                exitCode = NAGIOS_WARNING
            elif (voltage <= opts.critical):
                exitCode = NAGIOS_CRITICAL
            else:
                exitCode = NAGIOS_OK
    finally:
        sock.close()

    sys.exit(exitCode)

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--ip", type=str, required=True, help="Sensor IP address")
    parser.add_argument("-p", "--port", type=int, default=5050, help="Sensor TCP port")
    parser.add_argument("-a", "--auth", type=str, default="ArduinoNano", help="Sensor authentication password")
    parser.add_argument("-o", "--command", type=str, default="Q", help="Sensor command")
    parser.add_argument("-c", "--critical", type=float, default=23.1, help="Critical voltage level")
    parser.add_argument("-w", "--warning", type=float, default=23.9, help="Warning voltage level")

    parser.parse_args(namespace=opts)

    main()

