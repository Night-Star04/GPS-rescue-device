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
    led.toggle();
    delay(500);
}