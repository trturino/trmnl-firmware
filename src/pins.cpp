#include <Arduino.h>
#include <pins.h>
#include <config.h>
#include <Wire.h>

void pins_init(void)
{
    pinMode(PIN_INTERRUPT, INPUT);
    pinMode(PIN_RESET, INPUT_PULLUP);
    pinMode(PIN_QWIICPOWER, OUTPUT);
    digitalWrite(PIN_QWIICPOWER, HIGH);

    Wire.begin();
}

void pins_set_clear_interrupt(void (*f)(void))
{
    attachInterrupt(PIN_RESET, f, RISING);
}