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
    for (uint8_t i = 0; i < 255; i++)
    {
        led.pwm(i);
        delay(10);
    }
    for (uint8_t i = 255; i > 0; i--)
    {
        led.pwm(i);
        delay(10);
    }
}
