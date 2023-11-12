#include "../I2cBus.h"
#include "../I2cBusLinux.h"
#include "I2cBusLinuxWrapper.h"


struct I2cBusLinuxWrapper {
    I2cBusLinux* i2cBusLinux;
};

I2cBusLinuxWrapper* I2cBusLinuxWrapper_create(int busId, const char* name) {
    I2cBusLinuxWrapper* wrapper = (I2cBusLinuxWrapper*)malloc(sizeof(I2cBusLinuxWrapper));
    if (wrapper) {
        wrapper->i2cBusLinux = new I2cBusLinux(busId, name);
    }
    return wrapper;
}

void I2cBusLinuxWrapper_destroy(I2cBusLinuxWrapper* instance) {
    if (instance) {
        delete instance->i2cBusLinux;
        free(instance);
    }
}

int I2cBusLinuxWrapper_IsValid(I2cBusLinuxWrapper* instance) {
    return (instance && instance->i2cBusLinux && instance->i2cBusLinux->Valid());
}

void I2cBusLinuxWrapper_Reset(I2cBusLinuxWrapper* instance) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        instance->i2cBusLinux->reset();
    }
}

int I2cBusLinuxWrapper_ReadData(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return instance->i2cBusLinux->readData(devAddr, data, byteCount) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_ReadByte(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return instance->i2cBusLinux->readByte(devAddr, *data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteData(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount, unsigned int timeoutMsec) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return instance->i2cBusLinux->writeData(devAddr, data, byteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteByte(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char data) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return instance->i2cBusLinux->writeByte(devAddr, data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

int I2cBusLinuxWrapper_WriteReadData(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* command, unsigned int commandByteCount, unsigned char* data, unsigned int dataByteCount, unsigned int timeoutMsec) {
    if (I2cBusLinuxWrapper_IsValid(instance)) {
        return instance->i2cBusLinux->writeReadData(devAddr, command, commandByteCount, data, dataByteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}
