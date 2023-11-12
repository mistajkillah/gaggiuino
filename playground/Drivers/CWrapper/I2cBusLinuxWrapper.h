#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the C wrapper struct
typedef struct I2cBusLinuxWrapper I2cBusLinuxWrapper;

// Function to create an instance of the I2cBusLinuxWrapper
I2cBusLinuxWrapper* createI2cBusLinuxWrapper(int busId, const char* name);

// Function to destroy the I2cBusLinuxWrapper instance
void destroyI2cBusLinuxWrapper(I2cBusLinuxWrapper* instance);

// Function to check if the I2cBusLinuxWrapper is valid
bool isI2cBusLinuxValid(I2cBusLinuxWrapper* instance);

// Function to reset the I2cBusLinuxWrapper
void resetI2cBusLinux(I2cBusLinuxWrapper* instance);

// Function to read data from the I2cBusLinuxWrapper
bool readDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount);

// Function to read a byte from the I2cBusLinuxWrapper
bool readByteI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data);

// Function to write data to the I2cBusLinuxWrapper
bool writeDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount, unsigned int timeoutMsec);

// Function to write a byte to the I2cBusLinuxWrapper
bool writeByteI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char data);

// Function to write and read data from the I2cBusLinuxWrapper
bool writeReadDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* command, unsigned int commandByteCount, unsigned char* data, unsigned int dataByteCount, unsigned int timeoutMsec);

#ifdef __cplusplus
}
#endif
