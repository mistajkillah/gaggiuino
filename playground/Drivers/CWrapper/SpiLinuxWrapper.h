#pragma once

#include <stddef.h>

// Define a C struct to represent the SpiDeviceLinux (equivalent to C++ class)
typedef struct SpiDeviceLinuxWrapper SpiDeviceLinuxWrapper;

// Create an instance of the SpiDeviceLinux (equivalent to constructor)
SpiDeviceLinuxWrapper* SpiDeviceLinux_Create(int busId, int csIndex, int speedHz, long bitsPerWord, int delayUsec, int mode, const char* name);

// Destroy an instance of the SpiDeviceLinux (equivalent to destructor)
void SpiDeviceLinux_Destroy(SpiDeviceLinuxWrapper* device);

// Function to send and receive data using SpiDeviceLinux
int SpiDeviceLinux_SendReceiveBuffer(SpiDeviceLinuxWrapper* device, const unsigned char* sendMessage, size_t numberWriteBytes, unsigned char* responseMessage, size_t numberReadBytes);
