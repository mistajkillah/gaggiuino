#pragma once
//#include <stdint.h>



// Create an instance of the MAX6675_TempSensor (equivalent to constructor)
void* MAX6675_TempSensor_Create(void * spiDevice, const char* spiDeviceName) ;

// Destroy an instance of the MAX6675_TempSensor (equivalent to destructor)
void MAX6675_TempSensor_Destroy(void* sensor);

// Function to read temperature in Celsius using MAX6675_TempSensor
double MAX6675_TempSensor_readCelsius(void* sensor);

// Function to read temperature in Kelvin using MAX6675_TempSensor
double MAX6675_TempSensor_readKelvin(void* sensor);

// Function to read temperature in Fahrenheit using MAX6675_TempSensor
double MAX6675_TempSensor_readFahrenheit(void* sensor);
