#!/usr/bin/python
import socket
import sys

# Global options container
class Opts(object):
    pass

opts = Opts()

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)

    try:
        sock = socket.create_connection((opts.ip, opts.port))

    except socket.error, msg:
        print "Connection error: %s\n" % msg
        sys.exit(1)

    try:
        # Send data
        message = '%s,%s.' % (opts.auth, opts.command)
        print "Sending %s" % message
        sock.sendall(message)

        # Recieve data
        data = sock.recv(32)
        print "Received %s" % data

    except:
        print "Transmission error"

    finally:
        print "Closing socket"
        sock.close()

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    #parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")
    parser.add_argument("-i", "--ip", type=str, required=True, help="Sensor IP address")
    parser.add_argument("-p", "--port", type=int, default=5050, help="Sensor TCP port")
    parser.add_argument("-a", "--auth", type=str, default="ArduinoNano", help="Sensor authentication password")
    parser.add_argument("-c", "--command", type=str, default="Q", help="Sensor command")

    parser.parse_args(namespace=opts)

    main()

