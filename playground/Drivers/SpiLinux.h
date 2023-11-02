#pragma once
#include "Spi.h"
//class SpiBus;

class SPI;
class SpiBusLinux : public SpiBus
{
    const char* __class__ = "SpiLinux";

public:

    //typedef FpgaRegister::FpgaAddr FpgaAddr;
    //typedef FpgaRegister::FpgaData FpgaData;


    SpiBusLinux(int busId,
    int speedHz, 
    long bitsPerWord,     
    int delayUsec,
    int mode,
    const char* name);
    
    virtual ~SpiBusLinux()
    {}



 virtual  GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes);


//  virtual GenericDriverStatus reset(bool waitForReset = false);
 
    



private:
    int _busId = 0;
    int csIndex=0;   
    int speedHz=0;
    long bitsPerWord=0;
    int delayUsec=0;
    int mode=0;
    SPI *mySPI;

};

