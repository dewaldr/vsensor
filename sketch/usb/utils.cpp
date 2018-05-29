#include "utils.h"

void localBlink()
{
    pinMode(LOCAL_LED, OUTPUT);
    digitalWrite(LOCAL_LED, HIGH);
    delay(LOCAL_BLINK_MS);
    digitalWrite(LOCAL_LED, LOW);
}

void softReset() {
    asm volatile ("jmp 0x0000");
}

String getValue(const String& data, const char separator, const int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length()-1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return (found > index ? data.substring(strIndex[0], strIndex[1]) : "");
}

