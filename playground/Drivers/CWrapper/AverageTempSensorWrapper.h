#pragma once

// Define a C struct to represent the AverageTempSensor (equivalent to C++ class)
typedef struct AverageTempSensorWrapper AverageTempSensorWrapper;

typedef struct TempSensorWrapper TempSensorWrapper;
// Create an instance of the AverageTempSensor (equivalent to constructor)
AverageTempSensorWrapper* AverageTempSensor_Create(TempSensorWrapper* origin, int readingsNumber, int delayTimeInMillis);

// Destroy an instance of the AverageTempSensor (equivalent to destructor)
void AverageTempSensor_Destroy(AverageTempSensorWrapper* sensor);

// Function to read temperature in Celsius using AverageTempSensor
double AverageTempSensor_readCelsius(AverageTempSensorWrapper* sensor);

// Function to read temperature in Kelvin using AverageTempSensor
double AverageTempSensor_readKelvin(AverageTempSensorWrapper* sensor);

// Function to read temperature in Fahrenheit using AverageTempSensor
double AverageTempSensor_readFahrenheit(AverageTempSensorWrapper* sensor);

