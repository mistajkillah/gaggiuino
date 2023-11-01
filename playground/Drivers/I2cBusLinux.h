
#pragma once

#include "I2cBus.h"


/**
 * I2C bus accessed by Linux driver.
 */
class I2cBusLinux : public I2cBus
{
    const char* __class__ = "I2cBusLinux";

public:
    I2cBusLinux(int busId, const char* name) :
        I2cBus(name),
        _busId(busId)
    {
    }

    virtual ~I2cBusLinux();

    virtual bool Valid() { return true; }

    virtual GenericDriverStatus reset();

    virtual GenericDriverStatus readData(unsigned char deviceAddr,
                                unsigned char* data,
                                unsigned int byteCount);

    virtual GenericDriverStatus readByte(unsigned char deviceAddr,
                                unsigned char& data);

    virtual GenericDriverStatus writeData(unsigned char deviceAddr,
                                 unsigned char* data,
                                 unsigned int byteCount,
                                 unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC);

    virtual GenericDriverStatus writeByte(unsigned char deviceAddr,
                                 unsigned char data);

    virtual GenericDriverStatus writeReadData(unsigned char deviceAddr,
                                     unsigned char* command,
                                     unsigned int commandByteCount,
                                     unsigned char* data,
                                     unsigned int dataByteCount,
                                     unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC);

private:
    int _busId = 0;
    int i2c_bus_open(int busId);
    void i2c_bus_close(int busFD);
};

