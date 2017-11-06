# SolarSaver sketch Makefile

# Arduino.mk variables
ARDUINO_DIR			= /opt/arduino
ARDMK_DIR			= /opt/arduino-mk
AVR_TOOLS_DIR		= /opt/arduino/hardware/tools/avr

TARGET				= vsensor
ARDUINO_LIBS		= LowPower

BOARD_TAG			= uno
ARDUINO_PORT		= /dev/ttyACM0
MONITOR_BAUDRATE	= 9600

include /opt/arduino-mk/Arduino.mk

realclean: clean
	rm -f *~
	rm -rf build-*
