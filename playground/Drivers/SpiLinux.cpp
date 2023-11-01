  #include "GenericDrivers.h"
  #include "spidev_lib++.h"
  #include "SpiLinux.h"



    SpiLinux(int busId, const char* name) :
        Spi(name),
        _busId(busId),mySPU(NULL)
    {

    }

    
  GenericDriverStatus sendReceiveBuffer::sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes)
    {
      spi_config_t spi_config;
      spi_config.mode=0;
      spi_config.speed=1000000;
      spi_config.delay=0;
      spi_config.bits_per_word=8;
      char wbuf[32];
      int busFD = -1;
      sprintf(wbuf, "/dev/spidev%d.0", busId);
      mySPI=new SPI("/dev/spidev1.0",&spi_config);
    if(NULL == mySPI)
    {      
      mySPI=NULL;
      return GenericDriverStatus_SpiError;
    }
    if (mySPI->begin())
    {
      memset(tx_buffer,0,32);
      memset(rx_buffer,0,32);
      mySPI->xfer(sendMessage,numberWriteBytes,responseMessage,numberReadBytes));
    }
    else
    {
      delete mySPI
      mySPI=NULL;
      return GenericDriverStatus_SpiError;
    }
    }

    GenericDriverStatus sendReceiveBuffer::reset(bool waitForReset = false)
    {

    }
    