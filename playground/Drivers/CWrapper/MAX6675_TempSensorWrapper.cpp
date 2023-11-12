#include "../TempSensor.h"
#include "../MAX6675_TempSensor.h"
#include "TempSensorWrapper.h"
#include "SpiLinuxWrapper.h"
#include <stdlib.h>

struct MAX6675_TempSensorWrapper {
    MAX6675_TempSensor* cpp_sensor; // Pointer to the C++ MAX6675_TempSensor object
};

struct SpiDeviceLinuxWrapper {
    SpiDeviceLinux* cpp_device; // Pointer to the C++ SpiDeviceLinuxCpp object
};


MAX6675_TempSensorWrapper* MAX6675_TempSensor_Create(SpiDeviceLinuxWrapper * spiDevice, const char* spiDeviceName) {
    MAX6675_TempSensorWrapper* sensor = (MAX6675_TempSensorWrapper*)malloc(sizeof(MAX6675_TempSensorWrapper));
    if (sensor) {
        // Create and initialize the C++ MAX6675_TempSensor object
        sensor->cpp_sensor = new MAX6675_TempSensor(spiDevice->cpp_device,spiDeviceName);
    }
    return sensor;
}

void MAX6675_TempSensor_Destroy(MAX6675_TempSensorWrapper* sensor) {
    if (sensor) {
        // Clean up the C++ MAX6675_TempSensor object
        delete sensor->cpp_sensor;
        free(sensor);
    }
}

double MAX6675_TempSensor_readCelsius(MAX6675_TempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readCelsius method
        return sensor->cpp_sensor->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double MAX6675_TempSensor_readKelvin(MAX6675_TempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readKelvin method
        return sensor->cpp_sensor->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double MAX6675_TempSensor_readFahrenheit(MAX6675_TempSensorWrapper* sensor) {
    if (sensor && sensor->cpp_sensor) {
        // Call the C++ readFahrenheit method
        return sensor->cpp_sensor->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
