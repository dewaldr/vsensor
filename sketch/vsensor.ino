#include "LowPower.h"
#define SERIAL_BAUD      9600

const int siteID = 123;
const bool isSolarSite = false;
const bool debug = true;

// GPIO pins
// A0 = :0(
const int voltagePin = A1;
const int relayPin   = PD2;

void powerOn()
{
    digitalWrite(relayPin, HIGH);
}

void powerOff()
{
    digitalWrite(relayPin, LOW);
}

void setup() {
    pinMode(relayPin, INPUT_PULLUP);

    Serial.begin(SERIAL_BAUD);

    // Setup digital pin modes
    pinMode(relayPin, OUTPUT);
    powerOff();
}

void loop() {

    Serial.flush();

    // Enter idle state
    LowPower.idle(SLEEP_4S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

    int vReading = analogRead(voltagePin);

    // TODO: Select resistor values closer to 30V
    float vBattery = vReading * (27.1 / 1023);

    // TODO: Send on demand over UDP packet
    //       Encrypt UDP packet using XXTEA

    if (debug) {
        Serial.print("Site ID: ");
        Serial.print(siteID);
        Serial.print(", Reading: ");
        Serial.print(vReading);
        Serial.print(", Battery Voltage: ");
        Serial.println(vBattery);
    }
    else {
        Serial.print(siteID);
        Serial.print(",");
        Serial.println(vBattery);
    }

    // TODO: replace with proper algorithm
    if (vReading < 905 && digitalRead(relayPin)) {
        powerOff();
    }
    else if (vReading >= 905 ) {
        powerOn();
    }

}


