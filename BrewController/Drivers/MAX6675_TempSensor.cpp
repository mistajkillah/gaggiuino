/**
  Created by Yurii Salimov, February, 2018.
  Released into the public domain.
*/
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "Spi.h"
#include "Arduino.h"
#include "MAX6675_TempSensor.h"


double MAX6675_TempSensor::readCelsius() {
  GenericDriverStatus status=GenericDriverStatus_Success;
  int value=0;
  uint8_t tx_buffer[2];
  uint8_t rx_buffer[2];
  memset(tx_buffer, 0, 2);
  memset(rx_buffer, 0, 2);
  if(NULL == _spiDevice)
  {
    assert(0 && " spi device null");
  }
  if(GenericDriverStatus_Success != (status=_spiDevice->sendReceiveBuffer(tx_buffer, 2, rx_buffer, 2)))
  {
    assert(0 && "GenericDriverStatus");
    return -1;
  }
  value =((rx_buffer[0] <<8) &0xFF00) | (rx_buffer[1] &0x000FF);
  if (value & 0x4) 
  {
    return -1;
  }
  value >>= 3;
  return (value * 0.25);
}

/**
  Returns a temperature in Kelvin.
  Reads the temperature in Celsius,
  converts in Kelvin and return it.

  @return temperature in degree Kelvin
*/
double MAX6675_TempSensor::readKelvin() {
  return celsiusToKelvins(readCelsius());
}

/**
  Returns a temperature in Fahrenheit.
  Reads a temperature in Celsius,
  converts in Fahrenheit and return it.

  @return temperature in degree Fahrenheit
*/
double MAX6675_TempSensor::readFahrenheit() {
  return celsiusToFahrenheit(readCelsius());
}

byte MAX6675_TempSensor::spiread() {
  byte value = 0;

  

  return value;
}

inline double MAX6675_TempSensor::celsiusToKelvins(const double celsius) {
  return (celsius + 273.15);
}

inline double MAX6675_TempSensor::celsiusToFahrenheit(const double celsius) {
  return (celsius * 1.8 + 32);
}
