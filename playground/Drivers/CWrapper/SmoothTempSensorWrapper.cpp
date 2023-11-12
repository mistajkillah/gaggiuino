#include "../SmoothTempSensor.h"
#include "../TempSensor.h"
#include "TempSensorWrapper.h"
#include "SmoothTempSensorWrapper.h"
#include <stdlib.h>

struct SmoothTempSensorWrapper {
    TempSensor* cpp_sensor; // Pointer to the C++ SmoothTempSensorCpp object
};
struct TempSensorWrapper {
    TempSensor* cpp_sensor; // Pointer to the C++ AverageTempSensorCpp object
};

SmoothTempSensorWrapper* SmoothTempSensor_Create(TempSensorWrapper* origin, int smoothingFactor) {
    SmoothTempSensorWrapper* sensor = (SmoothTempSensorWrapper*)malloc(sizeof(SmoothTempSensorWrapper));
    if (sensor) {
        // Create and initialize the C++ SmoothTempSensorCpp object
        sensor->cpp_sensor = new SmoothTempSensor(origin->cpp_sensor, smoothingFactor);
    }
    return sensor;
}

void SmoothTempSensor_Destroy(SmoothTempSensorWrapper* sensor) {
    if (sensor) {
        // Clean up the C++ SmoothTempSensorCpp object
        delete sensor->cpp_sensor;
        free(sensor);
    }
}

double SmoothTempSensor_readCelsius(SmoothTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readCelsius method
        return sensor->cpp_sensor->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double SmoothTempSensor_readKelvin(SmoothTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readKelvin method
        return sensor->cpp_sensor->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double SmoothTempSensor_readFahrenheit(SmoothTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readFahrenheit method
        return sensor->cpp_sensor->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
