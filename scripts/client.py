#!/usr/bin/python
import socket
import sys
import select

# Global options container
class Opts(object):
    pass

opts = Opts()

def main():

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5.0)
    except socket.error, exc:
        print "Socket setup error: %s\n" % exc
        sys.exit(1)

    retry = 0
    while (retry < opts.retries):
        retry = retry + 1
        print "Try %d" % retry
        try:
            sock = socket.create_connection((opts.ip, opts.port))
        except socket.error, exc:
            print "Socket connection error: %s\n" % exc
            sys.exit(1)

        try:
            # Send data
            message = 'auth:%s,command:%s#' % (opts.auth, opts.command)
            print "Sending %s" % message
            sock.sendall(message)

            # Receive data
            sock.setblocking(0)
            ready = select.select([sock], [], [], 2)
            if ready[0]:
                data = sock.recv(128)
                print "Received %s" % data
                break

        except socket.error, exc:
            print "Transmission error: %s\n" % exc
        finally:
            sock.close()
            print "Socket closed"

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    #parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")
    parser.add_argument("-i", "--ip", type=str, required=True, help="Sensor IP address")
    parser.add_argument("-p", "--port", type=int, default=5050, help="Sensor TCP port")
    parser.add_argument("-a", "--auth", type=str, default="ArduinoNano", help="Sensor authentication password")
    parser.add_argument("-c", "--command", type=str, default="query", help="Sensor command")
    parser.add_argument("-r", "--retries", type=int, default="3", help="# of times to retry on error")

    parser.parse_args(namespace=opts)

    main()

