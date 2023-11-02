#pragma once

#include <mutex>
#include "GenergicDrivers.h"



extern unsigned int I2C_BUS_CONFIG;


/**
 * Abstract base class for I2C buses. Derived classes are:
 *      I2cBusLinux - Bus accessed by Linux driver.
 *      I2cBusFpga  - Bus accessed by FPGA registers.
 */
class I2cBus
{    const char *__class__ = "I2cBus";

public:

    /*
     * Bus lock class.
     */
    class Lock : public std::unique_lock<std::recursive_mutex>
    {
        const char* __class__ = "I2cBus::Lock";

    public:
        Lock(I2cBus* bus);
        ~Lock();

    private:
        const I2cBus* bus;
    };
  
    I2cBus(const char* name);

    virtual ~I2cBus() { }

    virtual bool Valid() { return false; }

    virtual GenericDriverStatus reset()
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual GenericDriverStatus readData(unsigned char devAddr,
                                unsigned char* data,
                                unsigned int byteCount)
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual GenericDriverStatus readByte(unsigned char devAddr,
                                unsigned char &data)
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual GenericDriverStatus writeData(unsigned char devAddr,
                                 unsigned char* data,
                                 unsigned int byteCount,
                                 unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC)
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual GenericDriverStatus writeByte(unsigned char devAddr,
                                 unsigned char data)
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual GenericDriverStatus writeReadData(unsigned char devAddr,
                                     unsigned char* command,
                                     unsigned int commandByteCount,
                                     unsigned char* data,
                                     unsigned int dataByteCount,
                                     unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC)
    {
        return GenericDriverStatus_NotImplemented;
    }

    virtual void setBusSpeed(bool fullSpeed)
    {
    
    }
    
    const char *getName()
    {
        return _name;
    }

    static I2cBus &Empty() { return s_empty; }

protected:
    const char *_name;
    const static unsigned short READY_TIMEOUT_MSEC = 40;
    const static unsigned short WRITE_TIMEOUT_MSEC = 40;
    const static unsigned short READ_TIMEOUT_MSEC  = 40;

private:
    std::recursive_mutex mutex;
    static I2cBus s_empty;
};

