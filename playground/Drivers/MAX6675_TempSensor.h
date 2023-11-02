/**
	MAX6675_TempSensor - class describes a set of methods
	for working with a TempSensor based on the MAX6675 driver
	and reading a temperature in Celsius, Fahrenheit and Kelvin.

	Instantiation:
		TempSensor* TempSensor = new MAX6675_TempSensor(
		SCK_pin, CS_pin, SO_pin
	);

	Read temperature:
	double celsius = TempSensor->readCelsius();
	double kelvin = TempSensor->readKelvin();
	double fahrenheit = TempSensor->readFahrenheit();

	v.1.1.2:
	- optimized calls of private methods.

	v.1.1.3:
	- Fixed bug in setReadingsNumber() method.

	v.1.1.4:
	- Removed deprecated init() method.

	v.2.0.0
	- implemented TempSensor interface;
	- removed methods for averaging result.

	v.2.0.1
	- optimized celsiusToFahrenheit() method;
	- updated documentation.

	https://github.com/YuriiSalimov/MAX6675_TempSensor

	Created by Yurii Salimov, February, 2018.
	Released into the public domain.
*/
#ifndef MAX6675_TempSensor_H
#define MAX6675_TempSensor_H
#include "Spi.h"
#include "TempSensor.h"

class MAX6675_TempSensor final : public TempSensor{

	private:
	SpiDeviceLinux * _bus;
  const char *_name;

	public:
		MAX6675_TempSensor(
    SpiDeviceLinux *spiDevice,
    const char* name) : SpiDeviceLinux(spiDevice),
    //SpiDeviceLinux(bus, csIndex,speedHz,bitsPerWord,delayUsec,mode, name)
        // _bus(bus),
        // _busId(busId),
        // csIndex(csIndex),
        // bitsPerWord(bitsPerWord),
        // delayUsec(delayUsec),
        // mode(mode), 
         _name(name)
    {
    }

		/**
			Reads a temperature in Celsius from the TempSensor.

			@return temperature in degree Celsius or
			NAN if no TempSensor attached
		*/
		double readCelsius();

		/**
			Reads a temperature in Kelvin from the TempSensor.

			@return temperature in degree Kelvin or
			NAN if no TempSensor attached
		*/
		double readKelvin();

		/**
			Reads a temperature in Fahrenheit from the TempSensor.

			@return temperature in degree Fahrenheit or
			NAN if no TempSensor attached
		*/
		double readFahrenheit();

	private:
		byte spiread();

		/**
			Celsius to Kelvin conversion:
			K = C + 273.15

			@param celsius - temperature in degree Celsius to convert
			@return temperature in degree Kelvin
		*/
		inline double celsiusToKelvins(double celsius);

		/**
			Celsius to Fahrenheit conversion:
			F = C * 1.8 + 32

			@param celsius - temperature in degree Celsius to convert
			@return temperature in degree Fahrenheit
		*/
		inline double celsiusToFahrenheit(double celsius);
};

#endif
