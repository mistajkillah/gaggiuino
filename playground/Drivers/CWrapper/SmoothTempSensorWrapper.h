#pragma once
// Define a C struct to represent the SmoothTempSensor (equivalent to C++ class)



// Create an instance of the SmoothTempSensor (equivalent to constructor)
void* SmoothTempSensor_Create(void* origin, int smoothingFactor);

// Destroy an instance of the SmoothTempSensor (equivalent to destructor)
void SmoothTempSensor_Destroy(void* sensor);

// Function to read temperature in Celsius using SmoothTempSensor
double SmoothTempSensor_readCelsius(void* sensor);

// Function to read temperature in Kelvin using SmoothTempSensor
double SmoothTempSensor_readKelvin(void* sensor);

// Function to read temperature in Fahrenheit using SmoothTempSensor
double SmoothTempSensor_readFahrenheit(void* sensor);
