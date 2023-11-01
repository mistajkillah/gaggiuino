#pragma once
#include "Spi.h"
#include "GenergicDrivers.h"


class SpiLinux : public Spi
{
    const char* __class__ = "SpiLinux";

public:

    //typedef FpgaRegister::FpgaAddr FpgaAddr;
    //typedef FpgaRegister::FpgaData FpgaData;


    SpiLinux(int busId, const char* name) :
        Spi(name),
        _busId(busId)
    {
    }

    virtual ~SpiLinux();



  GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes);

    GenericDriverStatus reset(bool waitForReset = false);
    



private:
    int _busId = 0;
    GenericDriverStatus sendBuffer(const unsigned char* message, size_t sendByteCount, size_t numberReadBytes = 0);
    GenericDriverStatus readBuffer(unsigned char* message, size_t readByteCount, size_t writeByteCount);
  SPI *mySPI;

};

