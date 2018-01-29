#define MACADDRESS    0x00,0x01,0x02,0x03,0x04,0xA1
#define MYIPADDR      192,168,11,65
#define MYIPMASK      255,255,255,0
#define MYDNS         192,168,11,10
#define MYGW          192,168,11,1
#define LISTENPORT    5050
#define SERIAL_BAUD   115200
#define MSG_SIZE      32

#include <UIPEthernet.h>

// Site config
const int siteID = 65;
const bool isAC = true;
String passphrase = String("ArduinoNano");

// Sensor config
const int voltagePin = A0;
const int relayPin   = PD5;
const int nsamples   = 10;

EthernetServer server = EthernetServer(LISTENPORT);

void setup() {

    uint8_t mac[6] = {MACADDRESS};
    uint8_t myIP[4] = {MYIPADDR};
    uint8_t myMASK[4] = {MYIPMASK};
    uint8_t myDNS[4] = {MYDNS};
    uint8_t myGW[4] = {MYGW};

    Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);

    server.begin();

    Serial.begin(SERIAL_BAUD);
}

void loop() {


    if (EthernetClient client = server.available()) 
    {
        if (client.available() > 0)  {
          
            char msg[MSG_SIZE];
            memset(msg, 0x00, MSG_SIZE);
            client.read(msg, MSG_SIZE);

            String request = String(msg);

            // Compose reply
            String reply;

            if (request.startsWith(passphrase)) {

                // Read voltage sensor
                int analogue = 0;
                for (int x = 0; x < nsamples; x++) {
                    analogue += analogRead(voltagePin);
                }
                float vReading = analogue / nsamples;
                float vBattery = vReading * (32.2 / 1023);

                char relayState = 'N';
                if (isAC) {
                    relayState = digitalRead(relayPin) ? 'C' : 'O';
                }

                reply = String(siteID, DEC) + "," + String(isAC ? "A" : "S") + "," + relayState + "," + String(vReading, 2) + "," + String(vBattery, 2);
            }
            else {
                reply = "ERROR";
            }

            // DEBUG
            Serial.print("Request: ");
            Serial.println(request);
            Serial.print("Reply: ");
            Serial.println(reply);

            client.write(reply.c_str(), reply.length());

        }
        client.stop();
    }
}

