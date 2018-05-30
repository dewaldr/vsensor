#!/usr/bin/python
import socket
import sys
import select

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

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5.0)
    except socket.error, exc:
        print "Socket setup error: %s\n" % exc
        sys.exit(exitCode)

    response = ""
    retry = 0
    while (retry < opts.retries):
        retry += 1
        try:
            sock = socket.create_connection((opts.ip, opts.port))

        except socket.error, exc:
            print "Network connection error: %s" % exc
            sys.exit(exitCode)

        try:
            request = 'auth:%s,command:%s#' % (opts.auth, opts.command)
            sock.sendall(request)

            # Wait until data available, then receive
            sock.setblocking(0)
            ready = select.select([sock], [], [], 5)
            if ready[0]:
                response = sock.recv(MAX_MSG_SIZE)
                break

        except socket.error, exc:
            print "Network comms error: %s" % exc

        finally:
            sock.close()

    if ((len(response) > 10) and response.endswith('#')):

        # Drop trailing '#' and split
        data = (response[:-1]).split(',')
        status = 'unknown'
        relay = 'unknown'
        temperature = 0.0
        voltage = 0.0

        for snippet in data:
            reading = snippet.split(':')
            if reading[0] == 'status':
                status = reading[1]
            elif reading[0] == 'relay':
                relay = reading[1]
            elif reading[0] == 'temperature':
                temperature = float(reading[1])
            elif reading[0] == 'voltage':
                voltage = float(reading[1])
            else:
                status = "response_error"

        if (status != "query_ok"):
            print "Sensor query error: %s" % status
            exitCode = NAGIOS_UNKNOWN

        elif ((voltage <= opts.warning) and (voltage > opts.critical)):
            print "temperature: %.1f, voltage WARNING: %.1f" % (temperature, voltage)
            exitCode = NAGIOS_WARNING

        elif (voltage <= opts.critical):
            print "temperature: %.1f, voltage CRITICAL: %.1f" % (temperature, voltage)
            exitCode = NAGIOS_CRITICAL

        else:
            print "temperature: %.1f, voltage OK: %.1f" % (temperature, voltage)
            exitCode = NAGIOS_OK

    sys.exit(exitCode)

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--ip", type=str, required=True, help="Sensor IP address")
    parser.add_argument("-p", "--port", type=int, default=5050, help="Sensor TCP port [5050]")
    parser.add_argument("-a", "--auth", type=str, default="ArduinoNano", help="Sensor authentication password")
    parser.add_argument("-o", "--command", type=str, default="query", help="Sensor command [query]")
    parser.add_argument("-r", "--retries", type=int, default="3", help="# of times to retry")
    parser.add_argument("-c", "--critical", type=float, default=23.1, help="Critical voltage level [23.0]")
    parser.add_argument("-w", "--warning", type=float, default=23.9, help="Warning voltage level [23.9]")

    parser.parse_args(namespace=opts)

    main()

