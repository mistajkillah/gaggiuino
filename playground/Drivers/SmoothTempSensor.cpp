/**
  Created by Yurii Salimov, May, 2019.
  Released into the public domain.
*/
#include <stddef.h>
#include<algorithm> 
#include <math.h>
#include "Arduino.h"
#include "SmoothTempSensor.h"

SmoothTempSensor::SmoothTempSensor(
  TempSensor* origin,
  const int smoothingFactor
) {
  this->origin = origin;
  setSmoothingFactor(smoothingFactor);
}

SmoothTempSensor::~SmoothTempSensor() {
  delete this->origin;
}

double SmoothTempSensor::readCelsius() {
  return this->celsius = smoothe(
    this->origin->readCelsius(),
    this->celsius
  );
}

double SmoothTempSensor::readKelvin() {
  return this->kelvin = smoothe(
    this->origin->readKelvin(),
    this->kelvin
  );
}

double SmoothTempSensor::readFahrenheit() {
  return this->fahrenheit = smoothe(
    this->origin->readFahrenheit(),
    this->fahrenheit
  );
}

double SmoothTempSensor::smoothe(
  const double input, const double data
) {
  return (data == 0) ? input :
    ((data * (this->smoothingFactor - 1) + input) / this->smoothingFactor);
}

/**
  See about the max(*) function:
  https://www.arduino.cc/reference/en/language/functions/math/max/
*/
inline void SmoothTempSensor::setSmoothingFactor(const int smoothingFactor) {
  this->smoothingFactor = std::max(smoothingFactor, MIN_SMOOTHING_FACTOR);
}
