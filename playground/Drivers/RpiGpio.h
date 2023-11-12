#include "Gpio.h"
#include "GenericDrivers.h"

class RpiGpio : public Gpio
{
public:
  enum RpiGpioId
  {
    //RPI_GPIO_0,
    //RPI_GPIO_1,
    //RPI_GPIO_2,
    //RPI_GPIO_3,
    RPI_GPIO_4 = 4,
    RPI_GPIO_5 = 5,
    RPI_GPIO_6 = 6,
    //RPI_GPIO_7,
    //RPI_GPIO_8,
    //RPI_GPIO_9,
    //RPI_GPIO_10,
    //RPI_GPIO_11,
    RPI_GPIO_12 = 12,
    RPI_GPIO_13 = 13,
    //RPI_GPIO_14,
    //RPI_GPIO_15,
    RPI_GPIO_16 = 16,
    RPI_GPIO_17 = 17,
    RPI_GPIO_18 = 18,
    RPI_GPIO_19 = 19,
    RPI_GPIO_20 = 20,
    RPI_GPIO_21 = 21,
    RPI_GPIO_22 = 22,
    RPI_GPIO_23 = 23,
    RPI_GPIO_24 = 24,
    RPI_GPIO_25 = 25,
    RPI_GPIO_26 = 26
  };

  class RpiPin : public Gpio::Pin
  {
    RpiPin(
      Gpio& device,
      unsigned char position,
      Direction direction = Direction::INPUT,
      State activeState = State::LOW,
      PuPdState pupd = PuPd_None,
      unsigned  reset = 0xff);
      
  };

  RpiGpio(const char* name, unsigned  direction, unsigned  reset);

  virtual ~RpiGpio();
};