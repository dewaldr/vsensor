#!/usr/bin/python
import sys, serial, time

# Nagios exit codes
NAGIOS_OK       = 0
NAGIOS_WARNING  = 1
NAGIOS_CRITICAL = 2
NAGIOS_UNKNOWN  = 3

# Global options container
class Opts(object):
    pass

opts = Opts()

def main():

    exitCode = NAGIOS_UNKNOWN

    # Create and configure serial object
    # timeout values:
    #    1. None: wait forever, block call
    #    2. 0: non-blocking mode, return immediately
    #    3. x, x is bigger than 0, float allowed, timeout block call

    ser = serial.Serial()
    ser.port = opts.serialdevice
    ser.baudrate = opts.baudrate
    ser.bytesize = serial.EIGHTBITS     # number of bits per bytes
    ser.parity = serial.PARITY_NONE     # set parity check: no parity
    ser.stopbits = serial.STOPBITS_ONE  # number of stop bits
    ser.timeout = 2                     # non-block read
    ser.xonxoff = False                 # disable software flow control
    ser.rtscts = False                  # disable hardware (RTS/CTS) flow control
    ser.dsrdtr = False                  # disable hardware (DSR/DTR) flow control
    ser.writeTimeout = 2                # timeout for write

    try:
        ser.open()
    except Exception, e:
        print "vSensor: error opening serial port: %s" % opts.serialdevice
        sys.exit(exitCode)

    if ser.isOpen():

        try:
            ser.flushInput()
            ser.flushOutput()

            response = ser.readline()
            #write data
            if (opts.debug): print("writing serial data: command:query#")
            ser.write("command:query#\r")
            time.sleep(1)  #give the serial port time to receive the data

            response = ser.readline()
            if (opts.debug): print("read serial data: " + response)

            ser.close()

        except Exception, e1:
            print "vSensor: error communicating over serial port: %s" % opts.serialdevice
            sys.exit(exitCode)

    # Strip trailing whitespace, including newline
    response = response.rstrip()
    if ((len(response) > 10) and response.endswith('#')):

        # Drop trailing '#' and split
        data = (response[:-1]).split(',')
        status = 'unknown'
        voltage = 0.0

        for snippet in data:
            reading = snippet.split(':')
            if reading[0] == 'status':
                status = reading[1]
            elif reading[0] == 'voltage':
                voltage = float(reading[1])
            else:
                status = "response_error"

        if (status != "query_ok"):
            print "Sensor query error: %s" % status
            exitCode = NAGIOS_UNKNOWN

        elif ((voltage <= opts.warning) and (voltage > opts.critical)):
            print "voltage WARNING: %.1f" % voltage
            exitCode = NAGIOS_WARNING

        elif (voltage <= opts.critical):
            print "voltage CRITICAL: %.1f" % voltage
            exitCode = NAGIOS_CRITICAL

        else:
            print "voltage OK: %.1f" % voltage
            exitCode = NAGIOS_OK
    else:
        print "Sensor query response error"
        exitCode = NAGIOS_UNKNOWN

    sys.exit(exitCode)

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--serialdevice", type=str, default="/dev/ttyUSB0", help="Serial device [/dev/ttyUSB0]")
    parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baud rate [115200]")
    parser.add_argument("-d", "--debug", action='store_true', help="Debug serial protocol [False]")
    parser.add_argument("-o", "--command", type=str, default="query", help="Sensor command [query]")
    parser.add_argument("-r", "--retries", type=int, default="3", help="# of times to retry")
    parser.add_argument("-c", "--critical", type=float, default=23.2, help="Critical voltage level [23.2]")
    parser.add_argument("-w", "--warning", type=float, default=23.9, help="Warning voltage level [23.9]")

    parser.parse_args(namespace=opts)

    main()

