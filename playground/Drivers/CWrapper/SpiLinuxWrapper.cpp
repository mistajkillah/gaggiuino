#include "../SpiLinux.h"
#include "SpiLinuxWrapper.h"
#include <stdlib.h>


void* SpiDeviceLinux_Create(int busId, int csIndex, int speedHz, long bitsPerWord, int delayUsec, int mode, const char* name) {
    void * device =(void*) new SpiDeviceLinux(busId, csIndex, speedHz, bitsPerWord, delayUsec, mode, name);    
    return device;
}

void SpiDeviceLinux_Destroy(void* device) {
    if (device) {
        // Clean up the C++ SpiDeviceLinuxCpp object
        
        free(device);
    }
}

int SpiDeviceLinux_SendReceiveBuffer(void* device, const unsigned char* sendMessage, int numberWriteBytes, unsigned char* responseMessage, int numberReadBytes) {
    if (device ) {
        // Call the C++ sendReceiveBuffer method
        return ((SpiDeviceLinux*)device)->sendReceiveBuffer(sendMessage, numberWriteBytes, responseMessage, numberReadBytes);
    }
    return -1; // Placeholder, replace with error handling if needed
}
