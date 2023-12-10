#include <stdint.h>
#include "malloc.h"
#include "I2cBusLinuxWrapper.h"
#include "../ADS1X15.h"
#include "ADS1X15Wrapper.h"




ADS1X15Wrapper createADS1X15Wrapper(I2cBusLinuxWrapperHandle bus, char devAddr, const char* name) {
    ADS1X15Wrapper wrapper = (ADS1X15Wrapper)new ADS1X15((I2cBus*)bus, devAddr, name);

    return wrapper;
}


void destroyADS1X15Wrapper(ADS1X15Wrapper instance) {
    if (instance) {
        //delete instance;
        free(instance);
    }
}

void resetADS1X15Wrapper(ADS1X15Wrapper instance) {
    if (instance) {
        ((ADS1X15*)instance)->reset();
    }
}

int beginADS1X15Wrapper(ADS1X15Wrapper instance) {
    return (instance && ((ADS1X15*)instance)->begin());
}

int isConnectedADS1X15Wrapper(ADS1X15Wrapper instance) {
    return (instance && ((ADS1X15*)instance)->isConnected());
}

void setGainADS1X15Wrapper(ADS1X15Wrapper instance, char gain) {
    if (instance) {
        ((ADS1X15*)instance)->setGain(gain);
    }
}

char getGainADS1X15Wrapper(ADS1X15Wrapper instance) {
    if (instance) {
        return ((ADS1X15*)instance)->getGain();
    }
    return 0xFF; // Invalid gain error
}

// Implement the rest of the wrapper functions similarly...
