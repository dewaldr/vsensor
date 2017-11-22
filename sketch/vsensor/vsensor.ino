#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <LowPower.h>
#define SERIAL_BAUD     115200

// Site config
const int siteID = 65;
const bool isSolar = false;
String passphrase = String("ArduinoNano");

// Network config
byte macAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA };
IPAddress ipAddress(192, 168, 11, 65);
const unsigned int localPort = 5050;                // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];          // buffer to hold incoming packet,
EthernetUDP Udp;

// Sensor config
const int voltagePin = A0;
const int relayPin   = PD5;
const int nsamples   = 10;

void powerOn()
{
    digitalWrite(relayPin, HIGH);
}

void powerOff()
{
    digitalWrite(relayPin, LOW);
}

void setup() {
    // Setup relay
    pinMode(relayPin, INPUT_PULLUP);
    pinMode(relayPin, OUTPUT);
    powerOff();

    // Disable SD card
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    // Start the Ethernet and UDP:
    Ethernet.begin(macAddress, ipAddress);
    Udp.begin(localPort);

    Serial.begin(SERIAL_BAUD);
}

void loop() {

    Serial.flush();

    // Enter idle state
    LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_ON, USART0_OFF, TWI_OFF);

    // Read voltage sensor
    int analogue = 0;
    for (int x = 0; x < nsamples; x++) {
        analogue += analogRead(voltagePin);
    }
    float vReading = analogue / nsamples;
    float vBattery = vReading * (32.2 / 1023);

    char relayState = 'N';
    if (isSolar) {
        relayState = digitalRead(relayPin) ? 'C' : 'O';
    }

    // If network request received, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {

        IPAddress remoteIP = Udp.remoteIP();
        unsigned int remotePort = Udp.remotePort();

        // read the packet into packetBufffer
        Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

        String request = String(packetBuffer);

        // TODO: verify input

        // Compose reply
        String reply;

        if (request.startsWith(passphrase)) {
            reply = (String(siteID, DEC) + "," + String(isSolar ? "S" : "A") + \
            "," + relayState + "," + String(vReading, 2) + "," + String(vBattery, 2));
        }
        else {
            reply = "ERROR";
        }

        // DEBUG
        Serial.print("Request: ");
        Serial.println(packetBuffer);
        Serial.print("Reply: ");
        Serial.println(reply);

        // Send a reply to the IP address and port that sent us the packet we received
        Udp.beginPacket(remoteIP, remotePort);
        Udp.write(reply.c_str());
        Udp.endPacket();
    }
}
