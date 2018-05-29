#ifndef _UTILS_H_
#define _UTILS_H_

#include <Arduino.h>

#define LOCAL_LED           9                      // pin # of local LED
#define LOCAL_BLINK_MS      20                     // # of ms to blink local LED

void localBlink();
void softReset();
String getValue(const String& data, const char separator, const int index);

#endif  // _UTILS_H_
