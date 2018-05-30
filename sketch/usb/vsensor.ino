#include "utils.h"

#define FW_VERSION    "0.5"
#define SERIAL_BAUD   115200
#define MAX_MSG_SIZE  128
#define CVOLTAGE      30.3

// Globals - tsk, tsk
String __input = "";          // a string to hold incoming data
boolean __inputDone = false;  // whether the input is complete

// Sensor config
const int voltagePin = A0;
const int nsamples   = 10;    // Number of times to read analogue sensors

float readVoltage() {
    int total = 0;
    for (int x = 0; x < nsamples; x++) {
        total += analogRead(voltagePin);
        delay(4);
    }

    return ((total / nsamples) * (CVOLTAGE / 1024));
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
                // Work around broken embedded sprintf
                char voltage[5];
                dtostrf(readVoltage(), 2, 1, voltage);
                sprintf(reply, "status:query_ok,voltage:%s#", voltage);
            }
            else if (cmd.compareTo("version") == 0) {
                sprintf("status:command_ok,version:%s#", FW_VERSION);
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
