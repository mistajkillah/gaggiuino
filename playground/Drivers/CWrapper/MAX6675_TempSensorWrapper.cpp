#include "../TempSensor.h"
#include "../MAX6675_TempSensor.h"
#include "SpiLinuxWrapper.h"
#include <stdlib.h>


void* MAX6675_TempSensor_Create(void * spiDevice, const char* spiDeviceName) {
    void* sensor = new MAX6675_TempSensor((SpiBus*)spiDevice,spiDeviceName);
    
    return sensor;
}

void MAX6675_TempSensor_Destroy(void* sensor) {
    if (sensor) {
        // Clean up the C++ MAX6675_TempSensor object
        //delete sensor->cpp_sensor;
        free(sensor);
    }
}

double MAX6675_TempSensor_readCelsius(void* sensor) {
     if (sensor) {
        // Call the C++ readCelsius method
        return ((MAX6675_TempSensor*)sensor)->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double MAX6675_TempSensor_readKelvin(void* sensor) {
     if (sensor) {
        // Call the C++ readKelvin method
        return ((MAX6675_TempSensor*)sensor)->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double MAX6675_TempSensor_readFahrenheit(void* sensor) {
     if (sensor) {
        // Call the C++ readFahrenheit method
        return ((MAX6675_TempSensor*)sensor)->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
