/**
	TempSensor - interface describes a set of methods
	for working with a TempSensor sensor and reading
	a temperature in Celsius, Fahrenheit and Kelvin.

	v.2.0.0
	- created

	v.2.0.2
	- added virtual destructor

	https://github.com/YuriiSalimov/MAX6675_TempSensor

	Created by Yurii Salimov, May, 2019.
	Released into the public domain.
*/
#ifndef TempSensor_H
#define TempSensor_H


class TempSensor {

	public:
		/**
      Destructor
      Deletes TempSensor instance.
    */
		virtual ~TempSensor() {};

		/**
			Reads a temperature in Celsius from the TempSensor.

			@return temperature in degree Celsius
		*/
		virtual double readCelsius() = 0;

		/**
			Reads a temperature in Kelvin from the TempSensor.

			@return temperature in degree Kelvin
		*/
		virtual double readKelvin() = 0;

		/**
			Reads a temperature in Fahrenheit from the TempSensor.

			@return temperature in degree Fahrenheit
		*/
		virtual double readFahrenheit() = 0;
};

#endif
