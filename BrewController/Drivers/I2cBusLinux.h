
#pragma once

// class I2cBus{
//   public:
//   I2cBus(const char * name);
// virtual GenericDriverStatus readData(unsigned char devAddr,
//                                 unsigned char* data,
//                                 unsigned int byteCount);

//     virtual GenericDriverStatus readByte(unsigned char devAddr,
//                                 unsigned char &data);

//     virtual GenericDriverStatus writeData(unsigned char devAddr,
//                                  unsigned char* data,
//                                  unsigned int byteCount,
//                                  unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC);

//     virtual GenericDriverStatus writeByte(unsigned char devAddr,
//                                  unsigned char data);

//     virtual GenericDriverStatus writeReadData(unsigned char devAddr,
//                                      unsigned char* command,
//                                      unsigned int commandByteCount,
//                                      unsigned char* data,
//                                      unsigned int dataByteCount,
//                                      unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC);
//   protected:
//                                     const static unsigned short WRITE_TIMEOUT_MSEC = 40;
// };


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

    virtual GenericDriverStatus readData(unsigned char devAddr,
                                unsigned char* data,
                                unsigned int byteCount);

    virtual GenericDriverStatus readByte(unsigned char devAddr,
                                unsigned char& data);

    virtual GenericDriverStatus writeData(unsigned char devAddr,
                                 unsigned char* data,
                                 unsigned int byteCount,
                                 unsigned int timeoutMsec = WRITE_TIMEOUT_MSEC);

    virtual GenericDriverStatus writeByte(unsigned char devAddr,
                                 unsigned char data);

    virtual GenericDriverStatus writeReadData(unsigned char devAddr,
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
