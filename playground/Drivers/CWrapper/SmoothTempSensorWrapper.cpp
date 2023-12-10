#include <stdlib.h>
#include "../SmoothTempSensor.h"
#include "../TempSensor.h"
#include "SmoothTempSensorWrapper.h"



void* SmoothTempSensor_Create(void* origin, int smoothingFactor) {
    void * sensor=(void *)new SmoothTempSensor((TempSensor*)origin, smoothingFactor);
 
    return sensor;
}

void SmoothTempSensor_Destroy(void* sensor) {
    if (sensor) {
        // Clean up the C++ SmoothTempSensorCpp object
        //delete sensor;
        free(sensor);
    }
}

double SmoothTempSensor_readCelsius(void* sensor) {
    if (sensor) {
        // Call the C++ readCelsius method
        return ((SmoothTempSensor*)sensor)->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double SmoothTempSensor_readKelvin(void* sensor) {
    if (sensor) {
        // Call the C++ readKelvin method
        return ((SmoothTempSensor*)sensor)->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double SmoothTempSensor_readFahrenheit(void* sensor) {
    if (sensor) {
        // Call the C++ readFahrenheit method
        return ((SmoothTempSensor*)sensor)->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
