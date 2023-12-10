
#include <stdlib.h>
#include "../TempSensor.h"
#include "../AverageTempSensor.h"

#include "AverageTempSensorWrapper.h"

//#include <stdlib.h>

void* AverageTempSensor_Create(void* origin, int readingsNumber, int delayTimeInMillis) {
void * sensor = new AverageTempSensor((TempSensor*)origin, readingsNumber, delayTimeInMillis);

    return sensor;
}

void AverageTempSensor_Destroy(void* sensor) {
   if (sensor) {

        free(sensor);
    }
}

double AverageTempSensor_readCelsius(void* sensor) {
   if (sensor) {
        // Call the C++ readCelsius method
        return ((AverageTempSensor*)sensor)->readCelsius();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double AverageTempSensor_readKelvin(void* sensor) {
   if (sensor) {
        // Call the C++ readKelvin method
        return ((AverageTempSensor*)sensor)->readKelvin();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}

double AverageTempSensor_readFahrenheit(void* sensor) {
   if (sensor) {
        // Call the C++ readFahrenheit method
        return ((AverageTempSensor*)sensor)->readFahrenheit();
    }
    return 0.0; // Placeholder, replace with error handling if needed
}
