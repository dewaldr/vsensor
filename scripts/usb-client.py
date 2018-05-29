#!/usr/bin/python

import serial, time
#initialization and open the port

#possible timeout values:
#    1. None: wait forever, block call
#    2. 0: non-blocking mode, return immediately
#    3. x, x is bigger than 0, float allowed, timeout block call

ser = serial.Serial()
ser.port = "/dev/ttyUSB0"
ser.baudrate = 115200
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
#ser.timeout = None          #block read
ser.timeout = 1            #non-block read
#ser.timeout = 2              #timeout block read
ser.xonxoff = False     #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2     #timeout for write

try:
    ser.open()
except Exception, e:
    print "error open serial port: " + str(e)
    exit()

if ser.isOpen():

    try:
        ser.flushInput()
        ser.flushOutput()

        time.sleep(1)  #give the serial port sometime to receive the data
        response = ser.readline()
        time.sleep(1)  #give the serial port sometime to receive the data
        #write data
        print("write data: command:query#")
        ser.write("command:query#\r")
        time.sleep(1)  #give the serial port sometime to receive the data

        response = ser.readline()
        print("read data: " + response)

        ser.close()

    except Exception, e1:
        print "error communicating...: " + str(e1)

else:
    print "cannot open serial port "
