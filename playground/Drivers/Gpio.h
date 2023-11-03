#pragma once
#include"GenergicDrivers.h"

class Gpio
{
  const char* __class__ = "Gpio";
  const char* name;
public:

  class Pin
  {

  public:
    enum class Direction
    {
      INPUT = 0,
      OUTPUT = 1,
    };

    enum class State
    {
      LOW = 0,
      HIGH = 1,
    };

    enum PuPdState
    {
      PuPd_None=0,
      PuPd_PULL_UP =1,
      PuPd_PULL_DOWN=2,
    };

    Pin( 
      Gpio& device, 
      unsigned char position, 
      Direction direction = Direction::INPUT, 
      State activeState = State::LOW, 
      PuPdState pupd=PuPd_None, 
      unsigned  reset = 0xff);

    virtual GenericDriverStatus write(State value) = 0;
    virtual GenericDriverStatus read(State& value) = 0;
    GenericDriverStatus readCache(State& value);
    GenericDriverStatus isActive(bool& value);

protected:
    Gpio& _device;
    
    unsigned int pinPosition;

    Direction pinDirection;

    State pinActiveState;

    State pinDefaultState;
  };


  Gpio(const char* name,unsigned  direction, unsigned  reset);
  virtual ~Gpio();

  GenericDriverStatus reset();
  GenericDriverStatus configPin( 
    unsigned char position, 
    Gpio::Pin::Direction direction, 
    unsigned char resetState, 
    Gpio::Pin::State activeState, 
    Gpio::Pin::PuPdState pupd);

protected:
  unsigned  cache;
  unsigned char address;
  unsigned  pinDirections;
  unsigned  pinResets;
  unsigned  pinActiveStates;
  unsigned  cache;
  void addPin(unsigned char position, Gpio::Pin::Direction direction, unsigned char resetState, Gpio::Pin::State activeState,   Gpio::Pin::PuPdState pupd);

  virtual GenericDriverStatus configure() = 0;
  virtual GenericDriverStatus write(unsigned value) = 0;
  virtual GenericDriverStatus read(unsigned& value) = 0;
};
