#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the C wrapper struct
typedef void* I2cBusLinuxWrapperHandle;



// Function to create an instance of the I2cBusLinuxWrapper
I2cBusLinuxWrapperHandle I2cBusLinuxWrapper_Create(int busId, const char* name);

// Function to destroy the I2cBusLinuxWrapper instance
void I2cBusLinuxWrapper_Destroy(I2cBusLinuxWrapperHandle instance);

// Function to check if the I2cBusLinuxWrapper is valid
int I2cBusLinuxWrapper_IsValid(I2cBusLinuxWrapperHandle instance);

// Function to reset the I2cBusLinuxWrapper
void I2cBusLinuxWrapper_Reset(I2cBusLinuxWrapperHandle instance);

// Function to read data from the I2cBusLinuxWrapper
int I2cBusLinuxWrapper_ReadData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount);

// Function to read a byte from the I2cBusLinuxWrapper
int I2cBusLinuxWrapper_ReadByte(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data);

// Function to write data to the I2cBusLinuxWrapper
int I2cBusLinuxWrapper_WriteData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount, unsigned int timeoutMsec);

// Function to write a byte to the I2cBusLinuxWrapper
int I2cBusLinuxWrapper_WriteByte(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char data);

// Function to write and read data from the I2cBusLinuxWrapper
int I2cBusLinuxWrapper_WriteReadData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* command, unsigned int commandByteCount, unsigned char* data, unsigned int dataByteCount, unsigned int timeoutMsec);

#ifdef __cplusplus
}
#endif
