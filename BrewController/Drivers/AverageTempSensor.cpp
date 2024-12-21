/**
  Created by Yurii Salimov, May, 2019.
  Released into the public domain.
*/
#include <stddef.h>
#include "Arduino.h"
#include "GenericDrivers.h"
#include "AverageTempSensor.h"

AverageTempSensor::AverageTempSensor(
  TempSensor* origin,
  const int readingsNumber,
  const int delayTimeInMillis
) {
  this->origin = origin;
  this->readingsNumber = validate(readingsNumber, DEFAULT_READINGS_NUMBER);
  this->delayTime = validate(delayTimeInMillis, DEFAULT_DELAY_TIME);
}

AverageTempSensor::~AverageTempSensor() {
  delete this->origin;
}

double AverageTempSensor::readCelsius() {
  return average(&TempSensor::readCelsius);
}

double AverageTempSensor::readKelvin() {
return average(&TempSensor::readKelvin);
}

double AverageTempSensor::readFahrenheit() {
  return average(&TempSensor::readFahrenheit);
}

inline double AverageTempSensor::average(double (TempSensor::*read)()) {
  double sum = 0;
  for (int i = 0; i < this->readingsNumber; ++i) {
    sum += (this->origin->*read)();
    sleep();
  }
  return (sum / this->readingsNumber);
}

inline void AverageTempSensor::sleep() {
  delay(this->delayTime);
}

template <typename A, typename B>
inline A AverageTempSensor::validate(A data, B alternative) {
  return (data > 0) ? data : alternative;
}
