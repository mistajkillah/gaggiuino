#include "GenericDrivers.h"
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
    sprintf(handle, "/dev/spidev%d.%d", this->_busId, _csIndex);
}

GenericDriverStatus SpiDeviceLinux::sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
  unsigned char* responseMessage, size_t numberReadBytes) {
  if(mySPI == NULL)
  {
    mySPI = new SPI(handle, &spi_config);
  }
  if (NULL == mySPI)
  {
    mySPI = NULL;
    return GenericDriverStatus_SpiError;
  }
  if (mySPI->begin())
  {
    size_t status=0;
    if(numberReadBytes != (status=mySPI->xfer(sendMessage, numberWriteBytes, responseMessage, numberReadBytes)))
    {
      printf("ERROR:spi xfer status = 0x%lx\n", status);      
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

SpiDeviceLinux::~SpiDeviceLinux()
{
   if (NULL != mySPI)
  {
    delete mySPI;
    mySPI = NULL;    
  }
}

