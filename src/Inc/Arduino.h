#ifndef ARDUINO_H
#define ARDUINO_H

#define FALLING 1
#define RISING 0
#define LOW 0
#define HIGH 1
#define INPUT 1
#define INPUT_PULLUP 0
#define OUTPUT 0
#define TIM9 0
#define NOT_AN_INTERRUPT 0

static void attachInterrupt(int a,void fun(), int c)
{

};
static int digitalPinToInterrupt(int a)
{
  return 0;
};
static void digitalWrite(int one, int two)
{

};
static int digitalRead(int one)
{
return 0;
};

static void pinMode(int one , int two)
{
  
};
static void delay(int delay)
{

};
static void delayMicroseconds(int delay)
{
  
};
static int millis()
{
  return 0;
};
class SPIClass {       // The class
  public:             // Access specifier
    SPIClass(int a, int b , int c)
    {
      
    }
    int myNum;        // Attribute (int variable)
    
};

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



#endif