#include <Arduino.h>

#include "LED.h"
LED led(9);

void setup()
{
    Serial.begin(9600);
    led.begin();
}

void loop()
{
    led.on();
    delay(1000);
    led.off();
    delay(1000);
}