 #include "GenergicDrivers.h"
 #include "Spi.h"

 #include "SpiLinux.h"
 #include "MAX6675_TempSensor.h"
#include <pigpio.h>
#include <iostream>

const int inputPin = 22;    //PIN 15 interupt pin
const int outputPin = 27;   //PIN 13 response to interupt pin
const int togglePin = 17;   //PIN 11 fake clock/interupt agressor

// Callback function to be called when the input pin changes state
void inputCallback(int gpio, int level, uint32_t tick) {
    // Set the output pin based on the input pin level
    gpioWrite(outputPin, level);
}
void ReadTemp()
{
  
  SpiBusLinux spiBus(0,4000000,8,1000,0,"SpiBus0");
  MAX6675_TempSensor tempSensor(NULL,0,4000000,8,1000,0,"BoilerTemp");
}
int main() {
    // Initialize pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio" << std::endl;
        return 1;
    }

    // Set GPIO modes
    gpioSetMode(inputPin, PI_INPUT);
    gpioSetMode(outputPin, PI_OUTPUT);
    gpioSetMode(togglePin, PI_OUTPUT);

    // Set pull-down resistor on the input pin
    gpioSetPullUpDown(inputPin, PI_PUD_DOWN);

    // Set the callback function for the input pin
    gpioSetAlertFunc(inputPin, inputCallback);

    // Set up for toggling the third pin at 120 Hz
    const int toggleFrequency = 120;  // Toggle frequency in Hz
    const uint32_t toggleInterval = 1000000 / toggleFrequency;  // Interval in microseconds
    uint32_t lastToggleTime = gpioTick();  // Last toggle time

    std::cout << "Starting loop, press CTRL+C to exit..." << std::endl;

    // Main loop
    while (1) {
        uint32_t currentTime = gpioTick();
        // Check if it's time to toggle the pin
        if (currentTime - lastToggleTime >= toggleInterval) {
            // Read the current state and write the inverse
            gpioWrite(togglePin, !gpioRead(togglePin));
            lastToggleTime = currentTime;
        }

        // Small delay to prevent 100% CPU utilization in the loop
        gpioDelay(100);  // Delay for 1 millisecond
    }

    // Cleanup the pigpio resources
    gpioTerminate();
    return 0;
}
