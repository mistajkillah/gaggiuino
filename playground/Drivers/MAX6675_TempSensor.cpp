/**
	Created by Yurii Salimov, February, 2018.
	Released into the public domain.
*/
#include <string.h>
#include "Spi.h"
#include "MAX6675_TempSensor.h"

	// MAX6675_TempSensor::MAX6675_TempSensor(
  //   SpiBus *bus,
  //   int csIndex,     
  //   int speedHz, 
  //   long bitsPerWord,     
  //   int delayUsec,
  //   int mode,
  //   const char* name) :
  //       _bus(bus),
  //       _busId(busId),
  //       csIndex(csIndex),
  //       bitsPerWord(bitsPerWord),
  //       delayUsec(delayUsec),
  //       mode(mode), 
  //       _name(name)
  //   {
  //   }
double MAX6675_TempSensor::readCelsius() {
	int value = spiread();
	value <<= 8;
	value |= spiread();

	if (value & 0x4) {
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
  uint8_t tx_buffer[1];
  uint8_t rx_buffer[1];
  memset(tx_buffer,0,1);
  memset(rx_buffer,0,1);	
	_bus->sendReceiveBuffer(tx_buffer,1,rx_buffer,1);
 
	return value;
}

inline double MAX6675_TempSensor::celsiusToKelvins(const double celsius) {
	return (celsius + 273.15);
}

inline double MAX6675_TempSensor::celsiusToFahrenheit(const double celsius) {
	return (celsius * 1.8 + 32);
}
