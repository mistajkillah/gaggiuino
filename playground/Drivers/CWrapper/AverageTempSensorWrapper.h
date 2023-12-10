#pragma once


// Create an instance of the AverageTempSensor (equivalent to constructor)
void* AverageTempSensor_Create(void* origin, int readingsNumber, int delayTimeInMillis);

// Destroy an instance of the AverageTempSensor (equivalent to destructor)
void AverageTempSensor_Destroy(void* sensor);

// Function to read temperature in Celsius using AverageTempSensor
double AverageTempSensor_readCelsius(void* sensor);

// Function to read temperature in Kelvin using AverageTempSensor
double AverageTempSensor_readKelvin(void* sensor);

// Function to read temperature in Fahrenheit using AverageTempSensor
double AverageTempSensor_readFahrenheit(void* sensor);

