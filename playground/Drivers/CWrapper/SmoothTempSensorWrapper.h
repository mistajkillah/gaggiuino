#pragma once
// Define a C struct to represent the SmoothTempSensor (equivalent to C++ class)
typedef struct SmoothTempSensorWrapper SmoothTempSensorWrapper;
typedef struct TempSensorWrapper TempSensorWrapper;

// Create an instance of the SmoothTempSensor (equivalent to constructor)
SmoothTempSensorWrapper* SmoothTempSensor_Create(TempSensorWrapper* origin, int smoothingFactor);

// Destroy an instance of the SmoothTempSensor (equivalent to destructor)
void SmoothTempSensor_Destroy(SmoothTempSensorWrapper* sensor);

// Function to read temperature in Celsius using SmoothTempSensor
double SmoothTempSensor_readCelsius(SmoothTempSensorWrapper* sensor);

// Function to read temperature in Kelvin using SmoothTempSensor
double SmoothTempSensor_readKelvin(SmoothTempSensorWrapper* sensor);

// Function to read temperature in Fahrenheit using SmoothTempSensor
double SmoothTempSensor_readFahrenheit(SmoothTempSensorWrapper* sensor);
