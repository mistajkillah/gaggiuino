
#include <pigpio.h>
#include <stddef.h>
#include "Arduino.h"
#include "GenericDrivers.h"
#include "PSM.h"
#ifdef __cplusplus
extern "C" {
#endif
PSM* _thePSM;

PSM::PSM(unsigned char sensePin, unsigned char controlPin, unsigned int range, int mode, unsigned char divider, unsigned char interruptMinTimeDiff) {
  _thePSM = this;

  
  _sensePin = sensePin;
  _mode=mode;
  
  _controlPin = controlPin;

  _divider = divider > 0 ? divider : 1;

  
  _range = range;
  _interruptMinTimeDiff = interruptMinTimeDiff;
}

void PSM::Initialize()
{   
  printf("Pump Intializing\n");
  
  gpioSetMode(_sensePin, PI_INPUT);
  gpioSetMode(_controlPin, PI_INPUT);
  gpioSetPullUpDown(_sensePin, PI_PUD_UP);
  gpioSetPullUpDown(_controlPin, PI_PUD_DOWN);
  gpioWrite(_controlPin, PI_LOW);

  // pinSetPullUpDown(_sensePin, INPUT_PULLUP);

  // pinMode(_controlPin, OUTPUT);


  int interruptNum = digitalPinToInterrupt(_sensePin);

  if (interruptNum != NOT_AN_INTERRUPT) {
    attachInterrupt(interruptNum, onInterrupt, _mode);
  }
}
void onPSMInterrupt() __attribute__((weak));
void onPSMInterrupt() {}

void PSM::onInterrupt(int gpio, int level, uint32_t tick){
  (void)gpio;
  (void)level;
  (void)tick;
  if ((_thePSM->_interruptMinTimeDiff > 0) && ((millis() - _thePSM->_lastMillis) < _thePSM->_interruptMinTimeDiff)) {
    //printf("interupt too quick");
    return;
  }
  _thePSM->_lastMillis = millis();

  onPSMInterrupt();

  if (_thePSM->_dividerCounter >= _thePSM->_divider) {
    _thePSM->_dividerCounter -= _thePSM->_divider;
    _thePSM->calculateSkip();
  } else {
    _thePSM->_dividerCounter++;
  }
}

void PSM::set(unsigned int value) {
  if (value < _range) {
    _value = value;
  } else {
    _value = _range;
  }
}

long PSM::getCounter() {
  return _counter;
}

void PSM::resetCounter() {
  _counter = 0;
}

void PSM::stopAfter(long counter) {
  _stopAfter = counter;
}

void PSM::calculateSkip() {
  _lastMillis = millis();
  //printf("calcualting skip\n");
  _a += _value;

  if (_a >= _range) {
    _a -= _range;
    _skip = false;
  } else {
    _skip = true;
  }

  if (_a > _range) {
    _a = 0;
    _skip = false;
  }

  if (!_skip) {
    _counter++;
  }

  if (!_skip
    && _stopAfter > 0
    && _counter > _stopAfter) {
    _skip = true;
  }

  updateControl();
}

void PSM::updateControl() {
  if (_skip) {
    digitalWrite(_controlPin, PI_LOW);
//    printf("pump low\n") ; 
  } else {
    digitalWrite(_controlPin, PI_HIGH);
  //  printf("pump high\n");
  }
}

unsigned int PSM::cps() {
  unsigned int range = _range;
  unsigned int value = _value;
  unsigned char divider = _divider;

  _range = 0xFFFF;
  _value = 1;
  _a = 0;
  _divider = 1;

  unsigned long stopAt = millis() + 1000;

  while (millis() < stopAt) {
    delay(0);
  }

  unsigned int result = _a;

  _range = range;
  _value = value;
  _a = 0;
  _divider = divider;

  return result;
}

unsigned long PSM::getLastMillis() {
  return _lastMillis;
}

unsigned char PSM::getDivider(void) {
  return _divider;
}

void PSM::setDivider(unsigned char divider) {
  _divider = divider > 0 ? divider : 1;
}

void PSM::shiftDividerCounter(char value) {
  _dividerCounter += value;
}

void PSM::initTimer(unsigned int freq, int timerInstance) {
  LOG_ERROR("NOT IMPLEMENTED INIT TIMER FUNCITON %d",1);
  (void)freq;
  (void)timerInstance;
}
#ifdef __cplusplus
}
#endif