#pragma once
#ifdef __cplusplus
extern "C" {
#endif
class PSM
{
public:
static const int FALLING=1;
static const int  RISING=0;
  PSM(unsigned char sensePin, unsigned char controlPin, unsigned int range, int mode = PSM::RISING, unsigned char divider = 1, unsigned char interruptMinTimeDiff = 0);

  void set(unsigned int value);

  long getCounter();
  void resetCounter();

  void stopAfter(long counter);

  unsigned int cps();
  unsigned long getLastMillis();

  unsigned char getDivider(void);
  void setDivider(unsigned char divider = 1);
  void shiftDividerCounter(char value = 1);

private:
  static inline void onInterrupt(int gpio, int level, uint32_t tick);
  void calculateSkip();
  void updateControl();

  unsigned char _sensePin;
  unsigned char _controlPin;
  unsigned int _range;
  unsigned char _divider;
  unsigned char _dividerCounter = 1;
  unsigned char _interruptMinTimeDiff;
  volatile unsigned int _value;
  volatile unsigned int _a;
  volatile bool _skip;
  volatile long _counter;
  volatile long _stopAfter;
  volatile unsigned long _lastMillis;
};
#ifdef __cplusplus
}
#endif