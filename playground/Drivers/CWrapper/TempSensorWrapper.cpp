#include "../TempSensor.h"
#include "TempSensorWrapper.h"
#include <stdlib.h>

struct TempSensorWrapper {
    TempSensor* cpp_sensor; // Pointer to the C++ AverageTempSensorCpp object
};
