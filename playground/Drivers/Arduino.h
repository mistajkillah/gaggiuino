#pragma once
#include <stddef.h>
#include <stdint.h>

#define LOW 0
#define HIGH 1
#define INPUT PI_INPUT
#define OUTPUT PI_OUTPUT
#define INPUT_PULLUP PI_PUD_UP
#define NOT_AN_INTERRUPT -1

void attachInterrupt(int pint,void callback(int gpio, int level, uint32_t tick), int c);
int digitalPinToInterrupt(int a);
int pinSetPullUpDown(int pin, int upDown);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
int pinMode(int pin , int mode);
void delay(long delay);
void delayMicroseconds(int delay);
unsigned int millis();

template<class T>
const T& constrain(const T& x, const T& a, const T& b) {
    if(x < a) {
        return a;
    }
    else if(b < x) {
        return b;
    }
    else
        return x;
}
