#include <UIPEthernet.h>
//#include <LowPower.h>

#define LISTENPORT    5050
#define MAX_MSG_SIZE  128
#define SERIAL_BAUD   115200

// TODO: store site config in EEPROM
// Site config
const int siteID     = 65;
const bool debug     = true;
String passphrase    = String("ArduinoNano");

// Sensor config
const int voltagePin = A0;
const int relayPin   = PD5;
const int nsamples   = 10;    // Number of times to read analogue sensors

// Global server object
EthernetServer server = EthernetServer(LISTENPORT);

void relayOn() {
    digitalWrite(relayPin, HIGH);
}

void relayOff() {
    digitalWrite(relayPin, LOW);
}

void relayToggle() {

    if (digitalRead(relayPin)) {
        relayOff();
    }
    else {
        relayOn();
    }
}

char relayState() {
    char rState = digitalRead(relayPin) ? 'C' : 'O';

    return rState;
}

float readVoltage() {
    int total = 0;
    for (int x = 0; x < nsamples; x++) {
        total += analogRead(voltagePin);
        delay(4);
    }

    return ((total / nsamples) * (32.11 / 1023)) - 0.1;
}

void startLog() {
    if (debug) { Serial.begin(SERIAL_BAUD); }
}

void flushLog() {
    if (debug) { Serial.flush(); }
}

void writeLog(const String& msg) {
    if (debug) { Serial.println(msg); }
}

void setup() {

    // Setup relay
    pinMode(relayPin, INPUT_PULLUP);
    pinMode(relayPin, OUTPUT);
    relayOff();

    // TODO: store network config in EEPROM
    uint8_t mac[6]    = {0x00,0x01,0x02,0x03,0x04,0xA1};
    uint8_t myIP[4]   = {192,168,11,65};
    uint8_t myMASK[4] = {255,255,255,0};
    uint8_t myDNS[4]  = {192,168,11,10};
    uint8_t myGW[4]   = {192,168,11,10};

    Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
    server.begin();

    startLog();
}

void loop() {

    flushLog();

    // Enter idle state
    // LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_ON, USART0_OFF, TWI_OFF);

    if (EthernetClient client = server.available()) 
    {
        if (client.available() > 0)  {

            char msg[MAX_MSG_SIZE];
            memset(msg, 0x00, MAX_MSG_SIZE);
            client.read(msg, MAX_MSG_SIZE);
            // Ensure the message is null terminated
            msg[MAX_MSG_SIZE] = 0x00;

            // TODO: verify input
            String request = String(msg);

            // Compose reply
            String reply;

            if (request.startsWith(passphrase) && request.endsWith(".")) {

                // Parse query
                char command = 'N';
                int index = request.indexOf(',');
                if (index > 0) {
                    command = request.charAt(index + 1);
                }

                // Commands: Q = query state, T = toggle relay, N = unknown
                switch (command) {
                    case 'Q': 

                        // siteID, relayState, batteryVoltage
                        reply = String(siteID, DEC) + "," + String(relayState()) + "," + String(readVoltage(), 1) + ".";
                        break;

                    case 'T':
                        relayToggle();
                        reply = String(siteID, DEC) + ",COMMAND-OK.";
                        break;

                    default:
                        reply = String(siteID, DEC) + ",COMMAND-ERROR.";
                        break;
                }

            }
            else {
                reply = String(siteID, DEC) + ",GENERAL-ERROR.";
            }

            writeLog("Request: " + request);
            writeLog("Reply: " + reply);

            client.write(reply.c_str(), reply.length());

        }
        client.stop();
    }
}
