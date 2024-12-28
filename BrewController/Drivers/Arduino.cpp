#include <chrono>
#include <unistd.h>
#include <time.h>
#include <stddef.h>
#include "stdint.h"
#include "GenericDrivers.h"
#include <pigpio.h>

void attachInterrupt(int inputPin,void callback(int gpio, int level, uint32_t tick), int c)
{
  (void) c;
  gpioSetAlertFunc(inputPin, callback);
  //LOG_ERROR("interupt attached to %d\n", inputPin);
}
int digitalPinToInterrupt(int pin)
{
  return pin;
}
int digitalWrite(int pin, int value)
{
  return gpioWrite(pin,value);
}
int digitalRead(int pin)
{
  return gpioRead(pin);
}
int pinMode(int pin , int mode)
{
  return gpioSetMode(pin,mode);
}
int pinSetPullUpDown(int pin, int upDown)
{
  return gpioSetPullUpDown(pin, upDown);
}

void delayMicroseconds(int delay)
{
    usleep(delay);
}


void delay(long msec)
{
  return delayMicroseconds(msec*1000);
}

unsigned long  millis() {
    static auto start_time = std::chrono::steady_clock::now(); // Static variable to store the start time
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return static_cast<unsigned long>(elapsed);
}

// unsigned int millis () {
//   struct timespec t ;
//   clock_gettime ( CLOCK_MONOTONIC_RAW , & t ) ; // change CLOCK_MONOTONIC_RAW to CLOCK_MONOTONIC on non linux computers
//   return t.tv_sec * 1000 + ( t.tv_nsec + 500000 ) / 1000000 ;
// }