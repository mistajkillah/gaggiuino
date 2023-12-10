#pragma once

//#include <stddef.h>



// Create an instance of the SpiDeviceLinux (equivalent to constructor)
void* SpiDeviceLinux_Create(int busId, int csIndex, int speedHz, long bitsPerWord, int delayUsec, int mode, const char* name);

// Destroy an instance of the SpiDeviceLinux (equivalent to destructor)
void SpiDeviceLinux_Destroy(void * device);

// Function to send and receive data using SpiDeviceLinux
int SpiDeviceLinux_SendReceiveBuffer(void * device, const unsigned char* sendMessage, int numberWriteBytes, unsigned char* responseMessage, int numberReadBytes);
