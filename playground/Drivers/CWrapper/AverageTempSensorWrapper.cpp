#include "../TempSensor.h"
#include "../AverageTempSensor.h"
#include "TempSensorWrapper.h"
#include "AverageTempSensorWrapper.h"

#include <stdlib.h>
struct TempSensorWrapper {
    TempSensor* cpp_sensor; // Pointer to the C++ AverageTempSensorCpp object
};

struct AverageTempSensorWrapper {
    AverageTempSensor* cpp_sensor; // Pointer to the C++ AverageTempSensorCpp object
};

AverageTempSensorWrapper* AverageTempSensor_Create(TempSensorWrapper* origin, int readingsNumber, int delayTimeInMillis) {
    AverageTempSensorWrapper* sensor = (AverageTempSensorWrapper*)malloc(sizeof(AverageTempSensorWrapper));
    if (sensor) {
        // Create and initialize the C++ AverageTempSensorCpp object
        sensor->cpp_sensor = new AverageTempSensor(origin->cpp_sensor, readingsNumber, delayTimeInMillis);
    }
    return sensor;
}

void AverageTempSensor_Destroy(AverageTempSensorWrapper* sensor) {
    if (sensor) {
        // Clean up the C++ AverageTempSensorCpp object
        delete sensor->cpp_sensor;
        free(sensor);
    }
}

double AverageTempSensor_readCelsius(AverageTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readCelsius method
        return sensor->cpp_sensor->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double AverageTempSensor_readKelvin(AverageTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readKelvin method
        return sensor->cpp_sensor->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double AverageTempSensor_readFahrenheit(AverageTempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readFahrenheit method
        return sensor->cpp_sensor->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
