#include "../I2cBus.h"
#include "../I2cBusLinux.h"
#include "I2cBusLinuxWrapper.h"



I2cBusLinuxWrapperHandle I2cBusLinuxWrapper_Create(int busId, const char* name) {
    I2cBusLinuxWrapperHandle wrapper= (I2cBusLinuxWrapperHandle)new I2cBusLinux(busId, name);
    
    return wrapper;
}

void I2cBusLinuxWrapper_Destroy(I2cBusLinuxWrapperHandle instance) {
    if (instance) {
        delete instance;
        free(instance);
    }
}

int I2cBusLinuxWrapper_IsValid(I2cBusLinuxWrapperHandle instance) {
    return (instance && ((I2cBusLinux*)instance)->Valid());
}

void I2cBusLinuxWrapper_Reset(I2cBusLinuxWrapperHandle instance) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        ((I2cBusLinux*)instance)->reset();
    }
}

int I2cBusLinuxWrapper_ReadData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return ((I2cBusLinux*)instance)->readData(devAddr, data, byteCount) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_ReadByte(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return ((I2cBusLinux*)instance)->readByte(devAddr, *data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount, unsigned int timeoutMsec) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return ((I2cBusLinux*)instance)->writeData(devAddr, data, byteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteByte(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char data) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return ((I2cBusLinux*)instance)->writeByte(devAddr, data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteReadData(I2cBusLinuxWrapperHandle instance, unsigned char devAddr, unsigned char* command, unsigned int commandByteCount, unsigned char* data, unsigned int dataByteCount, unsigned int timeoutMsec) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return ((I2cBusLinux*)instance)->writeReadData(devAddr, command, commandByteCount, data, dataByteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}
