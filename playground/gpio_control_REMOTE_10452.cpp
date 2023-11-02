 #include "GenergicDrivers.h"
 #include "Spi.h"

 #include "SpiLinux.h"
 #include "MAX6675_TempSensor.h"
#include <pigpio.h>
#include <iostream>

const int inputPin = 22;   // BCM GPIO 6
const int outputPin = 26;  // BCM GPIO 7

void ReadTemp()
{
  
  SpiBusLinux spiBus(0,4000000,8,1000,0,"SpiBus0");
  MAX6675_TempSensor tempSensor(NULL,0,4000000,8,1000,0,"BoilerTemp");
}
int main(void) {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio" << std::endl;
        return 1;
    }

    gpioSetMode(inputPin, PI_INPUT);
    gpioSetMode(outputPin, PI_OUTPUT);

    std::cout << "Starting loop..." << std::endl;

    while (1) {
        if (gpioRead(inputPin) == 1) {
            gpioWrite(outputPin, 1);
        } else {
            gpioWrite(outputPin, 0);
        }

        time_sleep(0.01);  // Small delay to prevent bouncing
    }

    gpioTerminate();
    return 0;
}
