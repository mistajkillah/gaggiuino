/*
 * I2cGpioDevice.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: parallels
 *
 *  Updated on March 20, 2021 to support the PCA9535
 */
#include <unistd.h>
#include <assert.h>
#include "GenergicDrivers.h"
#include "I2cGpio.h"

//#define BETWEEN_START_STOP_SLEEP 500000
#ifdef BETWEEN_START_STOP_SLEEP 
#define FUNKY_SLEEP_WORAROUND usleep(BETWEEN_START_STOP_SLEEP);
#else
#define FUNKY_SLEEP_WORAROUND 
#endif
Gpio::Pin::Pin(Gpio &device, unsigned char position, Gpio::Pin::Direction direction, Gpio::Pin::State activeState, unsigned  reset) :
    device(device)
{
    device.addPin(position, direction, reset, activeState);
}

GenericDriverStatus Gpio::Pin::write(GpioPin::State value)
{
    // unsigned int data = 0;

    // TER_Status status = device.read(data);
    // if(status != TER_Status_Success)
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
    // TER_Status status = device.read(data);
    // device.cache = data;
    // data &= (1 << pinPosition);
    // value = (data != 0) ? GpioPin::State::HIGH : GpioPin::State::LOW;

    // return status;
}


GenericDriverStatus Gpio::Pin::readCache(Gpio::Pin::State &value)
{
    value = ((device.cache & (1 << pinPosition)) != 0) ? Gpio::Pin::State::HIGH : Gpio::Pin::State::LOW;
    return TER_Status_Success;
}

GenericDriverStatus I2cGpio::Pin::isActive(bool& value)
{
  //   GpioPin::State gpioState;
	// readCache(gpioState);
	// GpioPin::State pinActiveState = ((device.pinActiveStates & (1 << pinPosition)) != 0) ? GpioPin::State::HIGH : GpioPin::State::LOW;
  //   switch (pinActiveState)
  //   {
  //   case GpioPin::State::LOW:
  //       value = (gpioState == GpioPin::State::LOW) ? true : false;
  //       break;
  //   case GpioPin::State::HIGH:
  //       value = (gpioState == GpioPin::State::HIGH) ? true : false;
  //       break;
  //   default:
  //       break;
  //   }

	// return TER_Status_Success;
}


I2cGpio::I2cGpio(I2cBus &bus, unsigned char address, unsigned  direction, unsigned  reset) :
    bus(bus),
    address(address),
    pinDirections(direction),
    pinResets(reset),
    cache(reset)
{
}


I2cGpio::~I2cGpio()
{
}

/*
  *
  * JMJ-doing this reset sequence breaks the asmedia card. And the board reboots multiple times.
  * reversed the sequence to write state then configure as output
  **/
TER_Status I2cGpio::reset()
{
    TER_Status status = TER_Status_Success;
    tmprintf_m(TMSK_i2c, "GPIO Pin Resets: %x \n" , (int)pinResets);
    if (TER_Status_Success == (status =write(pinResets)))
    {
        tmprintf_m(TMSK_i2c, "Now Configuring \n");
        return configure();
    }  
    return status;
}

TER_Status I2cGpio::configPin(unsigned char position, GpioPin::Direction direction, unsigned char resetState, GpioPin::State activeState)
{
	TER_Status status = TER_Status_Success;
    addPin(position, direction, resetState, activeState);
    return status;
}

void I2cGpio::addPin(unsigned char position, GpioPin::Direction direction, unsigned char pinReset, GpioPin::State activeState)
{
    unsigned mask = 1 << position;

    switch(direction)
    {
    case GpioPin::Direction::INPUT:
        pinDirections |= mask;
        break;
    case GpioPin::Direction::OUTPUT:
        pinDirections &= ~mask;
        break;
    }

    switch(pinReset)
    {
    case 1:
        pinResets |= mask;
        break;
    case 0:
        pinResets &= ~mask;
       break;
    }

    switch (activeState)
    {
    case GpioPin::State::HIGH:
        pinActiveStates |= mask;
        break;
    case GpioPin::State::LOW:
        pinActiveStates &= ~mask;
        break;
    default:
        break;
    }
}



/*
 * PCA9500
 */

GpioPCA9500::GpioPCA9500(I2cBus &bus, unsigned char address) :
        I2cGpio(bus, address, 0xff, 0xff)
{
}


GpioPCA9500::~GpioPCA9500()
{
}


TER_Status GpioPCA9500::configure()
{
    return TER_Status_Success;
}


TER_Status GpioPCA9500::write(unsigned  value)
{
    return bus.writeByte(address, value);
}


TER_Status GpioPCA9500::read(unsigned  &value)
{

    return bus.readByte(address, (unsigned char&)value);
}



/*
 * PCA9535
 */ 

GpioPCA9535::GpioPCA9535(I2cBus &bus, unsigned char address) :
        I2cGpio(bus, address, 0xff, 0xff)
{

}


GpioPCA9535::~GpioPCA9535()
{
}


TER_Status GpioPCA9535::configure()
{
    TER_Status status = TER_Status_Success;

    tmprintf(TMSK_i2c, "PCA9535::configure\nCONFIG: Writing polarity registers. Command: %02x %02x Value: %x \n",
        (int)CMD_POLARITY0, (int)CMD_POLARITY1, (int)0);
    status = write(CMD_POLARITY0, 0);
    if (TER_Status_Success != (status))
    {
        tmprintf_m(TMSK_error, "error configuring gpio polarity on port 0\n");
        return status;
    }
    FUNKY_SLEEP_WORAROUND
    if (TER_Status_Success != (status = write(CMD_POLARITY1, 0)))
    {
        tmprintf_m(TMSK_error, "error configuring gpio polarity on port 1\n");
        return status;
    }
    FUNKY_SLEEP_WORAROUND
    tmprintf(TMSK_i2c, "CONFIG: Writing config registers. Command: %02x %02x Value(pinDirections): %x \n", (int)CMD_CONFIG0, (int)CMD_CONFIG1, (int)pinDirections);
    status = write(CMD_CONFIG0, pinDirections);
    if (TER_Status_Success != (status))
    {
        tmprintf_m(TMSK_error, "error configuring gpio direction on port 0\n");
        return status;
    }
    FUNKY_SLEEP_WORAROUND
    tmprintf(TMSK_i2c, "CONFIG: Writing config registers. Command: %02x %02x Value(pinDirections): %x \n", (int)CMD_CONFIG0, (int)CMD_CONFIG1, (int)pinDirections>>8);
    status = write(CMD_CONFIG1, pinDirections>>8);
    if (TER_Status_Success != (status))
    {
        tmprintf_m(TMSK_error, "error configuring gpio direction on port 1\n");
        return status;
    }
    FUNKY_SLEEP_WORAROUND
    return tmstatus(TMSK_i2c, status);
}


TER_Status GpioPCA9535::write(unsigned value)
{
    TER_Status status = TER_Status_Success;

    tmprintf(TMSK_i2c, "PCA9535: (%d) write %04x\n", (unsigned) status, value);
    status = write(CMD_OUTPUT0, value);
    if (status != TER_Status_Success)
    {
        return status;
	
    }
    FUNKY_SLEEP_WORAROUND
    status = write(CMD_OUTPUT1, value >> 8);
    if (status != TER_Status_Success)
    {
        return status;
     
    }
    FUNKY_SLEEP_WORAROUND
    return tmstatus(TMSK_i2c, status);
}


TER_Status GpioPCA9535::write(unsigned char command, unsigned value)
{
    TER_Status status = TER_Status_Success;
    unsigned char data[2];

    data[0] = command;
    data[1] = value;
    tmprintf(TMSK_i2c, "PCA9535: (%d) write command %02x, %04x\n", (unsigned) status, command, value);
    status = bus.writeData(address, data, 2);

    return tmstatus(TMSK_i2c, status);
}


TER_Status GpioPCA9535::read(unsigned  &value)
{
    TER_Status status = TER_Status_Success;
    unsigned char cmd;
    unsigned char in0, in1;
    unsigned char out0, out1;
    unsigned int inValue = 0;
    unsigned int outValue = 0;
    tmprintf(TMSK_i2c, "PCA9535: (%d) read %04x\n", (unsigned) status, value);
    cmd = CMD_INPUT0;
    status = bus.writeReadData(address, &cmd, 1, &in0, 1);
    if (status != TER_Status_Success)
    {
        return tmstatus(TMSK_i2c, status);
    }
    cmd = CMD_INPUT1;
    status = bus.writeReadData(address, &cmd, 1, &in1, 1);
	if(status != TER_Status_Success)
	{
        return tmstatus(TMSK_i2c, status);
	}
    inValue = in0 | (in1 << 8);
    cmd = CMD_OUTPUT0;
    status = bus.writeReadData(address, &cmd, 1, &out0, 1);
    if (status != TER_Status_Success)
    {
        return tmstatus(TMSK_i2c, status);
    }
    cmd = CMD_OUTPUT1;
    status = bus.writeReadData(address, &cmd, 1, &out1, 1);
    if (status != TER_Status_Success)
    {
        return tmstatus(TMSK_i2c, status);
    }
    outValue = out0 | (out1 << 8);
    value = 0;
    for (int i = 0; i < 16; i++)
    {
        unsigned int pinDirectionBit = (pinDirections & (1 << i));
        if (pinDirectionBit > 0)//if it is 1 it is an input
        {
            value |= (inValue & (1 << i));
        }
        else//0 is an output pin.
        {
            value |= (outValue & (1 << i));
        }
        //todo this breaks if they change polarity.
    }
    return tmstatus(TMSK_i2c, status);
}


/*
 * PCA9554
 */

GpioPCA9554::GpioPCA9554(I2cBus &bus, unsigned char address) :
        I2cGpio(bus, address, 0xff, 0xff)
{
}


GpioPCA9554::~GpioPCA9554()
{
}


TER_Status GpioPCA9554::configure()
{
	TER_Status status = TER_Status_Success;
	
    tmprintf(TMSK_i2c, "CONFIG: Writing polarity registers. Command: %d Value: %x \n", (int)CMD_POLARITY, (int)0);
    status = write(CMD_POLARITY, 0);
    
    //Xprintf(-1, "CONFIG: Writing config registers. Command: %d Value(pinDirections): %x \n", (int)CMD_CONFIG, (int)pinDirections);
    //status = write(CMD_CONFIG, pinDirections);
	if(status == TER_Status_Success)
	{
		tmprintf(TMSK_i2c, "CONFIG: Writing config registers. Command: %d Value(pinDirections): %x \n", (int)CMD_CONFIG, (int)pinDirections);
    	status = write(CMD_CONFIG, pinDirections);
	}

    return status;
}

/*TER_Status GpioPCA9554::reset()
{
    TER_Status status = TER_Status_Success;
    //Xprintf(-1, "RESET: Writing pin outputs. Command: %d Value(pinResets): %x \n", (int)CMD_OUTPUT, (int)pinResets);
    status = write(CMD_OUTPUT, pinResets);

    configure();
    return status;
}*/

TER_Status GpioPCA9554::write(unsigned  value)
{
    return write(CMD_OUTPUT, value);
}


TER_Status GpioPCA9554::write(unsigned char command, unsigned char value)
{
    unsigned char data[] = { command, value };
    return bus.writeData(address, data, 2);
}


TER_Status GpioPCA9554::read(unsigned  &value)
{
    unsigned char cmd = CMD_INPUT;
    unsigned char data;

    TER_Status status = bus.writeReadData(address, &cmd, 1, &data, 1);

    if (status == TER_Status_Success)
    {
        value = data;
    }

    return status;
}

/*
 * PCA9574
 */

GpioPCA9574::GpioPCA9574(I2cBus& bus, unsigned char address) :
    I2cGpio(bus, address, 0xff, 0xff)
{
}


GpioPCA9574::~GpioPCA9574()
{
}

TER_Status GpioPCA9574::configure()
{
    TER_Status status = TER_Status_Success;

    tmprintf(TMSK_i2c, "CONFIG: Writing polarity register. Command: %d Value: %x \n", (int)CMD_POLARITY, pinActiveStates);
    status = write(CMD_POLARITY, pinActiveStates);

    if (status == TER_Status_Success)
    {
        tmprintf(TMSK_i2c, "CONFIG: Writing config register. Command: %d Value(pinDirections): %x \n", (int)CMD_CONFIG, (int)pinDirections);
        status = write(CMD_CONFIG, pinDirections);
    }

    if (status == TER_Status_Success)
    {
        tmprintf(TMSK_i2c, "CONFIG: Writing pinResets. Command: %d Value(pinResets): %x \n", (int)CMD_OUTPUT, (int)pinResets);
        status = write(CMD_OUTPUT, pinResets);
    }
    return status;
}

/*TER_Status GpioPCA9574::reset()
{
    TER_Status status = TER_Status_Success;
    //Xprintf(-1, "RESET: Writing pin outputs. Command: %d Value(pinResets): %x \n", (int)CMD_OUTPUT, (int)pinResets);
    status = write(CMD_OUTPUT, pinResets);

    configure();
    return status;
}*/

TER_Status GpioPCA9574::write(unsigned  value)
{
    return write(CMD_OUTPUT, value);
}


TER_Status GpioPCA9574::write(unsigned char command, unsigned char value)
{
    unsigned char data[] = { command, value };
    return bus.writeData(address, data, 2);
}


TER_Status GpioPCA9574::read(unsigned& value)
{
    unsigned char cmd = CMD_INPUT;
    unsigned char data;

    TER_Status status = bus.writeReadData(address, &cmd, 1, &data, 1);

    if (status == TER_Status_Success)
    {
        value = data;
    }

    return status;
}
