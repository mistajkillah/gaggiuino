#include <stdint.h>
#include "malloc.h"
#include "I2cBusLinuxWrapper.h"
#include "../ADS1X15.h"
#include "ADS1X15Wrapper.h"


struct ADS1X15Wrapper {
    ADS1X15* ads1x15;
};

ADS1X15Wrapper* createADS1X15Wrapper(I2cBusLinuxWrapper* bus, uint8_t devAddr, const char* name) {
    ADS1X15Wrapper* wrapper = (ADS1X15Wrapper*)malloc(sizeof(ADS1X15Wrapper));
    if (wrapper) {
        // Cast the I2cBusLinuxWrapper pointer to I2cBus*
        I2cBus* busPtr = (I2cBus*)bus;
        wrapper->ads1x15 = new ADS1X15(busPtr, devAddr, name);
    }
    return wrapper;
}


void destroyADS1X15Wrapper(ADS1X15Wrapper* instance) {
    if (instance) {
        delete instance->ads1x15;
        free(instance);
    }
}

void resetADS1X15Wrapper(ADS1X15Wrapper* instance) {
    if (instance) {
        instance->ads1x15->reset();
    }
}

bool beginADS1X15Wrapper(ADS1X15Wrapper* instance) {
    return (instance && instance->ads1x15->begin());
}

bool isConnectedADS1X15Wrapper(ADS1X15Wrapper* instance) {
    return (instance && instance->ads1x15->isConnected());
}

void setGainADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t gain) {
    if (instance) {
        instance->ads1x15->setGain(gain);
    }
}

uint8_t getGainADS1X15Wrapper(ADS1X15Wrapper* instance) {
    if (instance) {
        return instance->ads1x15->getGain();
    }
    return 0xFF; // Invalid gain error
}

// Implement the rest of the wrapper functions similarly...
