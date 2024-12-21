/**
  SmoothTempSensor - class-wrapper allows to smooth
  the temperature value of origin TempSensor instance.

  Instantiation:
  TempSensor* TempSensor = new SmoothTempSensor(
    TempSensor, SMOOTH_FACTOR
  );

  Where,
  TempSensor - origin TempSensor instance.
  SMOOTH_FACTOR - smoothing factor of a temperature value.

  Read temperature:
  double celsius = TempSensor->readCelsius();
  double kelvin = TempSensor->readKelvin();
  double fahrenheit = TempSensor->readFahrenheit();

  v.2.0.0
  - created

  v.2.0.1
  - optimized smoothe(*) method;
  - added default constant for the smoothing factor;
  - added default value of constructor parameters;
  - updated documentation.

  v.2.0.2
  - replaced "define" constants with "static const"

  https://github.com/YuriiSalimov/MAX6675_TempSensor

  Created by Yurii Salimov, May, 2019.
  Released into the public domain.
*/
#pragma once

#include "TempSensor.h"
#ifdef __cplusplus
extern "C" {
#endif
class SmoothTempSensor final : public TempSensor {

  private:
    // Minimum smoothing factor.
    //static const int MIN_SMOOTHING_FACTOR = 2;

    TempSensor* origin;
    int smoothingFactor;
    double celsius = 0;
    double kelvin = 0;
    double fahrenheit = 0;

  public:
    /**
      Constructor

      @param origin - origin TempSensor instance.
      @param smoothingFactor - smoothing factor of a temperature value
    */
    SmoothTempSensor(
      TempSensor* origin,
      int smoothingFactor
    );

    /**
      Destructor
      Deletes the origin TempSensor instance.
    */
    ~SmoothTempSensor();

    /**
      Reads a temperature in Celsius from the TempSensor.

      @return average temperature in degree Celsius
    */
    double readCelsius() override;

    /**
      Reads a temperature in Kelvin from the TempSensor.

      @return smoothed temperature in degree Kelvin
    */
    double readKelvin() override;

    /**
      Reads a temperature in Fahrenheit from the TempSensor.

      @return smoothed temperature in degree Fahrenheit
    */
    double readFahrenheit() override;

  private:
    /**
      Perform smoothing of the input value.

      @param input - the value to smooth
      @param data - the data for smoothing of the input value
      @return smoothed value or the input value
      if the input data is 0.
    */
    inline double smoothe(double input, double data);

    /**
      Sets the smoothing factor.
      If the input value is less than TempSensor_MIN_SMOOTHING_FACTOR,
      then sets TempSensor_MIN_SMOOTHING_FACTOR.

      @param smoothingFactor - new smoothing factor
    */
    inline void setSmoothingFactor(int smoothingFactor);
};

#ifdef __cplusplus
}
#endif