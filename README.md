# vsensor
Arduino-based power sensor for Wireless ISP remote sites

This project uses an Arduino Nano with an Ethernet shield to remotely sense the backup battery voltage at an ISP high site. 
The sensor is polled using a Nagios plugin; Nagios then sends notifications when low voltage is detected. A probe for Mikrotik 
TheDude is planned.
