#pragma once
#include <stdint.h>

// Define a C struct to represent the MAX6675_TempSensor (equivalent to C++ class)
typedef struct MAX6675_TempSensorWrapper MAX6675_TempSensorWrapper;

// Create an instance of the MAX6675_TempSensor (equivalent to constructor)
MAX6675_TempSensorWrapper* MAX6675_TempSensor_Create(const char* spiDeviceName);

// Destroy an instance of the MAX6675_TempSensor (equivalent to destructor)
void MAX6675_TempSensor_Destroy(MAX6675_TempSensorWrapper* sensor);

// Function to read temperature in Celsius using MAX6675_TempSensor
double MAX6675_TempSensor_readCelsius(MAX6675_TempSensorWrapper* sensor);

// Function to read temperature in Kelvin using MAX6675_TempSensor
double MAX6675_TempSensor_readKelvin(MAX6675_TempSensorWrapper* sensor);

// Function to read temperature in Fahrenheit using MAX6675_TempSensor
double MAX6675_TempSensor_readFahrenheit(MAX6675_TempSensorWrapper* sensor);
