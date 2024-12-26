#include "GenericDrivers.h"
#include "Spi.h"
 #include "I2cBus.h"
 #include "I2cBusLinux.h"
 #include "ADS1X15.h"

 #include "MAX6675_TempSensor.h"

#include <pigpio.h>
#include <sqlite3.h>

#include <iostream>



// Array to map physical pin to BCM GPIO pin
int rpiPinToBCM[41] = {
    -1,  // Index 0 (no pin 0, placeholder for 1-based indexing)
    -1,  // Pin 1: 3.3V
    -1,  // Pin 2: 5V
     2,  // Pin 3: GPIO2 (SDA1)
    -1,  // Pin 4: 5V
     3,  // Pin 5: GPIO3 (SCL1)
    -1,  // Pin 6: Ground
     4,  // Pin 7: GPIO4
    14,  // Pin 8: GPIO14 (TXD)
    -1,  // Pin 9: Ground
    15,  // Pin 10: GPIO15 (RXD)
    17,  // Pin 11: GPIO17
    18,  // Pin 12: GPIO18
    27,  // Pin 13: GPIO27
    -1,  // Pin 14: Ground
    22,  // Pin 15: GPIO22
    23,  // Pin 16: GPIO23
    -1,  // Pin 17: 3.3V
    24,  // Pin 18: GPIO24
    10,  // Pin 19: GPIO10 (MOSI)
    -1,  // Pin 20: Ground
     9,  // Pin 21: GPIO9 (MISO)
    25,  // Pin 22: GPIO25
    11,  // Pin 23: GPIO11 (SCLK)
     8,  // Pin 24: GPIO8 (CE0)
    -1,  // Pin 25: Ground
     7,  // Pin 26: GPIO7 (CE1)
    -1,  // Pin 27: ID_SD
    -1,  // Pin 28: ID_SC
     5,  // Pin 29: GPIO5
    -1,  // Pin 30: Ground
     6,  // Pin 31: GPIO6
    12,  // Pin 32: GPIO12
    13,  // Pin 33: GPIO13
    -1,  // Pin 34: Ground
    19,  // Pin 35: GPIO19
    16,  // Pin 36: GPIO16
    26,  // Pin 37: GPIO26
    20,  // Pin 38: GPIO20
    -1,  // Pin 39: Ground
    21   // Pin 40: GPIO21
};


const int inputPin = rpiPinToBCM[15]; //YELLOW //22;   // PIN 15 interupt pin
const int outputPin = rpiPinToBCM[13] ; //WHITE //27;  // PIN 13 response to interupt pin
//const int togglePin = rpiPinToBCM[11];//17;  Borken // PIN 11 fake clock/interupt agressor
const int togglePin = rpiPinToBCM[37];//RED



// Callback function to be called when the input pin changes state
void inputCallback(int gpio, int level, uint32_t tick) {
  // Set the output pin based on the input pin level
  gpioWrite(outputPin, level);
}
void ReadADC() {
  I2cBusLinux i2c(1, "i2c1");
  ADS1015 adc(&i2c, "WaterTemp", 0x48);
  adc.begin();
  adc.setGain(0);
  int16_t val_0 = adc.readADC(0);
  int16_t val_1 = adc.readADC(1);
  int16_t val_2 = adc.readADC(2);
  int16_t val_3 = adc.readADC(3);

  float f = adc.toVoltage(1);  // voltage factor

  printf("Analog0: 0x%X  %f\n", val_0, val_0 * f);
  printf("Analog1: 0x%X  %f\n", val_1, val_1 * f);
  printf("Analog2: 0x%X  %f\n", val_2, val_2 * f);
  printf("Analog3: 0x%X  %f\n", val_3, val_3 * f);
}

void ReadTemp() {
  SpiDeviceLinux spiDevice(0, 0, 4000000, 8, 1000, 0, "SpiDevice0.0");
  MAX6675_TempSensor tempSensor(&spiDevice, "BoilerTemp");
  printf("Temperature %lf\n", tempSensor.readCelsius());
}
int main(int argc, char* agv[]) {
  ReadTemp();
  ReadADC();


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
  const uint32_t toggleInterval =
      1000000 / toggleFrequency;         // Interval in microseconds
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
