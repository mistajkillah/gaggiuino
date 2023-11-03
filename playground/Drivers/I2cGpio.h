#pragma once
#include "I2cBus.h"
#include"GenergicDrivers.h"

class Gpio
{
    const char* __class__ ="Gpio";
    const char * name;
public:

    class Pin 
    {
        
    public:

    	enum class Direction
	{
		INPUT  = 0,
		OUTPUT = 1,
	};

	enum class State
	{
		LOW  = 0,
		HIGH = 1,
	};

        /// <summary>
        /// Construct the pin
        /// </summary>
        /// <param name="device">device of the GPIO</param>
        /// <param name="position">channel</param>
        /// <param name="direction">input or output</param>
        /// <param name="reset">default state after reset</param>
        /// <param name="pinProperty">active state</param>
        Pin(Gpio &device, unsigned char position, Direction direction = Direction::INPUT, State activeState = State::LOW, unsigned char reset = 0xff);

        virtual GenericStatus write(State value) =0;
        virtual GenericStatus read(State& value) =0;
        GenericStatus readCache(State& value);
        GenericStatus isActive(bool& value);

    private:
        Gpio &device;
    };


    Gpio(const char * name);
    virtual ~Gpio();

    GenericStatus reset();

    GenericStatus configPin(unsigned char position, Direction direction, unsigned char resetState, State activeState);

protected:
    unsigned  cache;

    void addPin(unsigned char position, GpioPin::Direction direction, unsigned char resetState, GpioPin::State activeState);

    virtual GenericStatus configure() = 0;
    virtual GenericStatus write(unsigned value) = 0;
    virtual GenericStatus read(unsigned  &value) = 0;
};
