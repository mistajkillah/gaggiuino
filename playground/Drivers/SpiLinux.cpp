#include "GenergicDrivers.h"
  #include "spidev_lib++.h"
  #include "SpiLinux.h"



    // SpiBusLinux(int busId, const char* name) :
    //     Spi(name),
    //     _busId(busId),mySPU(NULL)
    // {

    // }

        SpiBusLinux::SpiBusLinux(int busId,
    int speedHz, 
    long bitsPerWord,     
    int delayUsec,
    int mode,
    const char* name) :
        SpiBus(name),
        _busId(busId),
        bitsPerWord(bitsPerWord),
        delayUsec(delayUsec),
        mode(mode)
    {
      spi_config_t spi_config;
      spi_config.mode=mode;
      spi_config.speed=speedHz;
      spi_config.delay=delayUsec;
      spi_config.bits_per_word=bitsPerWord;
      char wbuf[32];
      int busFD = -1;
      sprintf(wbuf, "/dev/spidev%d.%d", this->_busId, 1) ;
      mySPI=new SPI(wbuf,&spi_config);

    }
  GenericDriverStatus SpiBusLinux::sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes)
    {
;
    if(NULL == mySPI)
    {      
      mySPI=NULL;
      return GenericDriverStatus_SpiError;
    }
    if (mySPI->begin())
    {
      memset(tx_buffer,0,32);
      memset(rx_buffer,0,32);
      mySPI->xfer(sendMessage,numberWriteBytes,responseMessage,numberReadBytes);
    }
    else
    {
      delete mySPI
      mySPI=NULL;
      return GenericDriverStatus_SpiError;
    }
    }


    