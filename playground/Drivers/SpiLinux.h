#pragma once
#include "Spi.h"
#include "spidev_lib++.h"
//class SpiBus;

class SpiDeviceLinux : public SpiBus
{
  const char* __class__ = "SpiLinux";

public:
  SpiDeviceLinux(int busId,
    int csIndex,
    int speedHz,
    long bitsPerWord,
    int delayUsec,
    int mode,
    const char* name);
  virtual  GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
    unsigned char* responseMessage, size_t numberReadBytes);
  
  virtual ~SpiDeviceLinux() {
  }
  
protected:
  int _busId;
  int _csIndex;
  int _speedHz;
  long _bitsPerWord;
  int _delayUsec;
  int _mode;
private:
  SPI* mySPI;
  char handle[32];
  spi_config_t spi_config;
};
