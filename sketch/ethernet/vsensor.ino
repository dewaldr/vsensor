//#include <avr/wdt.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <UIPEthernet.h>

#define FW_VERSION       "0.3"
#define LISTENPORT    5050
#define MAX_MSG_SIZE  128
#define ONE_WIRE_BUS  2

// TODO: - better detection for marformed commands
//       - XTEA encryption
//       - store site config in EEPROM
//       - store network config in EEPROM

// Site config
const bool debug  = true;
String deviceauth("ArduinoNano");

// Sensor config
const int voltagePin = A0;
const int relayPin   = PD5;
const int tempPin    = PD2;
const int nsamples   = 10;    // Number of times to read analogue sensors

OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensors(&oneWire);

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

uint8_t relayState() {
    return (digitalRead(relayPin));
}

float readVoltage() {
    int total = 0;
    float average = 0.0;
    for (int x = 0; x < nsamples; x++) {
        total += analogRead(voltagePin);
        delay(4);
    }

    average = ((total / nsamples) * (33.0 / 1023));
    return average;
}

float readTemp() {
    sensors.requestTemperatures();
    //delay(200);
    while (!sensors.isConversionComplete()); // wait until sensor is ready
    return sensors.getTempC();
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
    uint8_t myIP[4]   = {192,168,11,65};
    uint8_t myMASK[4] = {255,255,255,0};
    uint8_t myDNS[4]  = {192,168,11,10};
    uint8_t myGW[4]   = {192,168,11,1};

    Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
    server.begin();
    //wdt_enable(WDTO_2S);

    sensors.begin();
}

void loop() {

    //wdt_reset();
    //delay(500);

    if (EthernetClient client = server.available()) 
    {
        if (client.available() > 0)  {

            char msg[MAX_MSG_SIZE];
            memset(msg, '\0', MAX_MSG_SIZE);
            client.read(msg, MAX_MSG_SIZE);
            msg[MAX_MSG_SIZE - 1] = '\0';

            String request(msg);
            int reqlen = request.length();

            char reply[MAX_MSG_SIZE];
            memset(reply, '\0', MAX_MSG_SIZE);

            if (request.endsWith("#") && reqlen > 10) {
                request.remove(reqlen - 1);
                String authsnip = getSnippet(request, ',', 0);
                String auth = getSnippet(authsnip, ':', 1);

                if (auth.compareTo(deviceauth) == 0) {
                    String cmdsnip = getSnippet(request, ',', 1);
                    String cmd = getSnippet(cmdsnip, ':', 1);

                    if (cmd.compareTo("query") == 0) {
                        // Work around embedded sprintf
                        char temperature[5];
                        dtostrf(readTemp(), 2, 1, temperature);
                        char voltage[5];
                        dtostrf(readVoltage(), 2, 1, voltage);
                        sprintf(reply, "status:query_ok,relay:%d,temperature:%s,voltage:%s#", relayState(), temperature, voltage);
                    }
                    else if (cmd.compareTo("version") == 0) {
                        sprintf("status:command_ok,version:%s#", FW_VERSION);
                    }
                    else if (cmd.compareTo("toggle") == 0) {
                        relayToggle();
                        sprintf(reply, "status:command_ok#");
                    }
                    else {
                        sprintf(reply, "status:command_error#");
                    }
                }
                else {
                    sprintf(reply, "status:auth_error#");
                }
            }
            else {
                sprintf(reply, "status:general_error#");
            }

            client.write(reply, strlen(reply));
        }
        client.stop();
    }
}
