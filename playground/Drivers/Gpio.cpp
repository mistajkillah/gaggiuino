/*
 * GpioDevice.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: parallels
 *
 *  Updated on March 20, 2021 to support the PCA9535
 */
#include <unistd.h>
#include <assert.h>
#include "GenericDrivers.h"
#include "Gpio.h"

//#define BETWEEN_START_STOP_SLEEP 500000
#ifdef BETWEEN_START_STOP_SLEEP 
#define FUNKY_SLEEP_WORAROUND usleep(BETWEEN_START_STOP_SLEEP);
#else
#define FUNKY_SLEEP_WORAROUND 
#endif
Gpio::Pin::Pin( 
  Gpio &device, 
  unsigned char position, 
  Gpio::Pin::Direction direction, 
  Gpio::Pin::State activeState, 
  Gpio::Pin::PuPdState pupd, 
  unsigned  reset) :_device(device)
{
   device.addPin(position, direction, reset, activeState, pupd);
}

GenericDriverStatus Gpio::Pin::write(Gpio::Pin::State value)
{
    // unsigned int data = 0;

    // GenericDriverStatus status = device.read(data);
    // if(status != GenericDriverStatus_Success)
    // {
    //     return status;
    // }

    // data &= ~(1 << pinPosition);
    // data |= static_cast<unsigned int>(value) << pinPosition;

    // device.cache = data;
    // return device.write(data);
}


GenericDriverStatus Gpio::Pin::read(Gpio::Pin::State &value)
{
    // unsigned int data = 0;
    // GenericDriverStatus status = device.read(data);
    // device.cache = data;
    // data &= (1 << pinPosition);
    // value = (data != 0) ? Gpio::Pin::State::HIGH : Gpio::Pin::State::LOW;

    // return status;
}


GenericDriverStatus Gpio::Pin::readCache(Gpio::Pin::State &value)
{
    value = ((_device.cache & (1 << pinPosition)) != 0) ? Gpio::Pin::State::HIGH : Gpio::Pin::State::LOW;
    return GenericDriverStatus_Success;
}

GenericDriverStatus Gpio::Pin::isActive(bool& value)
{
  //   Gpio::Pin::State gpioState;
	// readCache(gpioState);
	// Gpio::Pin::State pinActiveState = ((device.pinActiveStates & (1 << pinPosition)) != 0) ? Gpio::Pin::State::HIGH : Gpio::Pin::State::LOW;
  //   switch (pinActiveState)
  //   {
  //   case Gpio::Pin::State::LOW:
  //       value = (gpioState == Gpio::Pin::State::LOW) ? true : false;
  //       break;
  //   case Gpio::Pin::State::HIGH:
  //       value = (gpioState == Gpio::Pin::State::HIGH) ? true : false;
  //       break;
  //   default:
  //       break;
  //   }

	// return GenericDriverStatus_Success;
}


Gpio::Gpio(const char* name,unsigned  direction, unsigned  reset):
    pinDirections(direction),
    pinResets(reset),
    cache(reset)
{
}


Gpio::~Gpio()
{
}

/*
  *
  * JMJ-doing this reset sequence breaks the asmedia card. And the board reboots multiple times.
  * reversed the sequence to write state then configure as output
  **/
GenericDriverStatus Gpio::reset()
{
    GenericDriverStatus status = GenericDriverStatus_Success;
    LOG_MASK_I2C( "GPIO Pin Resets: %x \n" , (int)pinResets);
    if (GenericDriverStatus_Success == (status =write(pinResets)))
    {
        LOG_MASK_I2C( "Now Configuring \n");
        return configure();
    }  
    return status;
}

GenericDriverStatus Gpio::configPin(unsigned char position, Gpio::Pin::Direction direction, unsigned char resetState, Gpio::Pin::State activeState,  Gpio::Pin::PuPdState pupd)
{
	GenericDriverStatus status = GenericDriverStatus_Success;
    addPin(position, direction, resetState, activeState, pupd);
    return status;
}

void Gpio::addPin(unsigned char position, Gpio::Pin::Direction direction, unsigned char pinReset, Gpio::Pin::State activeState,   Gpio::Pin::PuPdState pupd)
{
    unsigned mask = 1 << position;

    switch(direction)
    {
    case Gpio::Pin::Direction::INPUT:
        pinDirections |= mask;
        break;
    case Gpio::Pin::Direction::OUTPUT:
        pinDirections &= ~mask;
        break;
    }

    switch(pinReset)
    {
    case 1:
        pinReset |= mask;
        break;
    case 0:
        pinResets &= ~mask;
       break;
    }

    switch (activeState)
    {
    case Gpio::Pin::State::HIGH:
        pinActiveStates |= mask;
        break;
    case Gpio::Pin::State::LOW:
        pinActiveStates &= ~mask;
        break;
    default:
        break;
    }
}

