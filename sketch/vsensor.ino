#include <UIPEthernet.h>

#define LISTENPORT    5050
#define MAX_MSG_SIZE  128
#define SERIAL_BAUD   115200

String version("v0.2");

// TODO: store site config in EEPROM
// Site config
const bool debug  = true;
String deviceauth("ArduinoNano");

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

String relayState() {
    return (digitalRead(relayPin) ? "closed" : "open");
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

String getSnippet(const String& data, const char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if ((data.charAt(i) == separator) || (i == maxIndex)) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return (found > index ? data.substring(strIndex[0], strIndex[1]) : "");
}

void setup() {

    // Setup relay
    pinMode(relayPin, INPUT_PULLUP);
    pinMode(relayPin, OUTPUT);
    relayOff();

    // TODO: store network config in EEPROM
    uint8_t mac[6]    = {0x00,0x01,0x02,0x03,0x04,0xA0};
    uint8_t myIP[4]   = {192,168,1,65};
    uint8_t myMASK[4] = {255,255,255,0};
    uint8_t myDNS[4]  = {192,168,1,1};
    uint8_t myGW[4]   = {192,168,1,1};

    Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
    server.begin();

    startLog();
}

void loop() {

    flushLog();

    if (EthernetClient client = server.available()) 
    {
        if (client.available() > 0)  {

            char msg[MAX_MSG_SIZE];
            memset(msg, '\0', MAX_MSG_SIZE);
            client.read(msg, MAX_MSG_SIZE);
            msg[MAX_MSG_SIZE - 1] = '\0';

            String request(msg);
            int reqlen = request.length();
            writeLog("Request: " + request);

            String reply;

            if (request.endsWith("#") && reqlen > 10) {
                request.remove(reqlen - 1);
                String authsnip = getSnippet(request, ',', 0);
                String auth = getSnippet(authsnip, ':', 1);

                if (auth.compareTo(deviceauth) == 0) {
                    String cmdsnip = getSnippet(request, ',', 1);
                    String cmd = getSnippet(cmdsnip, ':', 1);

                    if (cmd.compareTo("query") == 0) {
                        reply = "status:query_ok,relay:" + String(relayState()) + ",voltage:" + String(readVoltage(), 1) + "#";
                    }
                    else if (cmd.compareTo("version") == 0) {
                        reply = "status:command_ok,version:" + version + "#";
                    }
                    else if (cmd.compareTo("toggle") == 0) {
                        relayToggle();
                        reply = "status:command_ok#";
                    }
                    else {
                        reply = "status:command_error#";
                    }
                }
                else {
                    reply = "status:auth_error#";
                }
            }
            else {
                reply = "status:general_error#";
            }

            writeLog("Reply: " + reply);

            client.write(reply.c_str(), reply.length());
        }
        client.stop();
    }
}
