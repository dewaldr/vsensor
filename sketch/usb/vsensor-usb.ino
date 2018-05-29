#include <OneWire.h>
#include <DS18B20.h>
#include "utils.h"

#define FW_VERSION    "0.4"
#define ONE_WIRE_BUS  2
#define SERIAL_BAUD   115200
#define MAX_MSG_SIZE   128

// Globals - tsk, tsk
String __input = "";          // a string to hold incoming data
boolean __inputDone = false;  // whether the input is complete

// Sensor config
const int voltagePin = A0;
const int relayPin   = PD5;
const int tempPin    = PD2;
const int nsamples   = 10;    // Number of times to read analogue sensors

OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensors(&oneWire);

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
    for (int x = 0; x < nsamples; x++) {
        total += analogRead(voltagePin);
        delay(4);
    }

    return ((total / nsamples) * (33.0 / 1023));
}

float readTemp() {
    sensors.requestTemperatures();
    //delay(200);
    while (!sensors.isConversionComplete()); // wait until sensor is ready
    return sensors.getTempC();
}

/*
 * SerialEvent occurs whenever a new data comes in the
 * hardware serial RX.  This routine is run between each
 * time loop() runs, so using delay inside loop can delay
 * response.  Multiple bytes of data may be available.
 */
void serialEvent() {
    while (Serial.available())
    {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        __input += inChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\r') {
            __inputDone = true;
        }
    }
}

void setup() {

    // Setup relay
    pinMode(relayPin, INPUT_PULLUP);
    pinMode(relayPin, OUTPUT);
    relayOff();

    sensors.begin();

    Serial.begin(SERIAL_BAUD);
    __input.reserve(MAX_MSG_SIZE);

    Serial.println("Started vsensor");
}

void loop() {

    if (__inputDone) {

        __input.trim();
        int inputLen = __input.length();

        char reply[MAX_MSG_SIZE];
        memset(reply, '\0', MAX_MSG_SIZE);

        if (__input.endsWith("#") && inputLen > 5) {

            __input.remove(inputLen - 1);

            String cmdsnip = getValue(__input, ',', 0);
            String cmd = getValue(cmdsnip, ':', 1);

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
            sprintf(reply, "status:general_error#");
        }

        Serial.println(reply);
        __inputDone = false;
        __input = "";
    }
}
