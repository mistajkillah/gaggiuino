#include "../SpiLinux.h"
#include "SpiLinuxWrapper.h"
#include <stdlib.h>

struct SpiDeviceLinuxWrapper {
    SpiDeviceLinux* cpp_device; // Pointer to the C++ SpiDeviceLinuxCpp object
};

SpiDeviceLinuxWrapper* SpiDeviceLinux_Create(int busId, int csIndex, int speedHz, long bitsPerWord, int delayUsec, int mode, const char* name) {
    SpiDeviceLinuxWrapper* device = (SpiDeviceLinuxWrapper*)malloc(sizeof(SpiDeviceLinuxWrapper));
    if (device) {
        // Create and initialize the C++ SpiDeviceLinuxCpp object
        device->cpp_device = new SpiDeviceLinux(busId, csIndex, speedHz, bitsPerWord, delayUsec, mode, name);
    }
    return device;
}

void SpiDeviceLinux_Destroy(SpiDeviceLinuxWrapper* device) {
    if (device) {
        // Clean up the C++ SpiDeviceLinuxCpp object
        delete device->cpp_device;
        free(device);
    }
}

int SpiDeviceLinux_SendReceiveBuffer(SpiDeviceLinuxWrapper* device, const unsigned char* sendMessage, size_t numberWriteBytes, unsigned char* responseMessage, size_t numberReadBytes) {
    if (device && device->cpp_device) {
        // Call the C++ sendReceiveBuffer method
        return device->cpp_device->sendReceiveBuffer(sendMessage, numberWriteBytes, responseMessage, numberReadBytes);
    }
    return -1; // Placeholder, replace with error handling if needed
}
