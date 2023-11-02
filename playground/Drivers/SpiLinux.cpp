#include "GenergicDrivers.h"
#include "spidev_lib++.h"
#include "SpiLinux.h"

SpiDeviceLinux::SpiDeviceLinux(int busId,
  int csIndex,
  int speedHz,
  long bitsPerWord,
  int delayUsec,
  int mode,
  const char* name) :
  SpiBus(name),
  _busId(busId),
  _csIndex(csIndex),
  _bitsPerWord(bitsPerWord),
  _delayUsec(delayUsec),
  _mode(mode) {

    spi_config.mode = mode;
    spi_config.speed = speedHz;
    spi_config.delay = delayUsec;
    spi_config.bits_per_word = bitsPerWord;
    sprintf(handle, "/dev/spidev%d.%d", this->_busId, 1);
}

GenericDriverStatus SpiDeviceLinux::sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
  unsigned char* responseMessage, size_t numberReadBytes) {
  mySPI = new SPI(handle, &spi_config);
  if (NULL == mySPI)
  {
    mySPI = NULL;
    return GenericDriverStatus_SpiError;
  }
  if (mySPI->begin())
  {
    if(0 !=mySPI->xfer(sendMessage, numberWriteBytes, responseMessage, numberReadBytes))
    {
      return GenericDriverStatus_SpiError;
    }
  }
  else
  {
    delete mySPI;
    mySPI = NULL;
   
  }
return  GenericDriverStatus_Success;
}


