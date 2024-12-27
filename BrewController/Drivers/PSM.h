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
  void Initialize();
  void set(unsigned int value);

  long getCounter();
  void resetCounter();

  void stopAfter(long counter);

  unsigned int cps();
  unsigned long getLastMillis();

  unsigned char getDivider(void);
  void setDivider(unsigned char divider = 1);
  void shiftDividerCounter(char value = 1);

  void initTimer(unsigned int freq, int timerInstance);

private:
  static inline void onInterrupt(int gpio, int level, uint32_t tick);
  void calculateSkip();
  void updateControl();

  unsigned char _sensePin=0;
  unsigned int  _mode=0;
  unsigned char _controlPin=0;
  unsigned int _range=0;
  unsigned char _divider=0;
  unsigned char _dividerCounter = 1;
  unsigned char _interruptMinTimeDiff=0;
  volatile unsigned int _value=0;
  volatile unsigned int _a=0;
  volatile bool _skip=0;
  volatile long _counter=0;
  volatile long _stopAfter=0;
  volatile unsigned long _lastMillis=0;

};
#ifdef __cplusplus
}
#endif
