/**
	Created by Yurii Salimov, February, 2018.
	Released into the public domain.
*/
#include "Spi.h"
#include "MAX6675_Thermocouple.h"

	MAX6675_Thermocouple::MAX6675_Thermocouple(Spi *bus, const char *name):
  _bus(bus),_name(name)
  {
  }

double MAX6675_Thermocouple::readCelsius() {
	//digitalWrite(this->CS_pin, LOW);
	delay(1);
	int value = spiread();
	value <<= 8;
	value |= spiread();
	//digitalWrite(this->CS_pin, HIGH);
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
double MAX6675_Thermocouple::readKelvin() {
	return celsiusToKelvins(readCelsius());
}

/**
	Returns a temperature in Fahrenheit.
	Reads a temperature in Celsius,
	converts in Fahrenheit and return it.

	@return temperature in degree Fahrenheit
*/
double MAX6675_Thermocouple::readFahrenheit() {
	return celsiusToFahrenheit(readCelsius());
}

byte MAX6675_Thermocouple::spiread() {
	byte value = 0;
	for (int i = 7; i >= 0; --i) {
		//digitalWrite(this->SCK_pin, LOW); //todo
		//delay(1);
		_bus->sendReceiveBuffer(NULL,NULL,NULL,NULL);
 
		//digitalWrite(this->SCK_pin, HIGH);
		//delay(1);
	}
	return value;
}

inline double MAX6675_Thermocouple::celsiusToKelvins(const double celsius) {
	return (celsius + 273.15);
}

inline double MAX6675_Thermocouple::celsiusToFahrenheit(const double celsius) {
	return (celsius * 1.8 + 32);
}
