#include "../I2cBus.h"
#include "../I2cBusLinux.h"
#include "I2cBusLinuxWrapper.h"


struct I2cBusLinuxWrapper {
    I2cBusLinux* i2cBusLinux;
};

I2cBusLinuxWrapper* createI2cBusLinuxWrapper(int busId, const char* name) {
    I2cBusLinuxWrapper* wrapper = (I2cBusLinuxWrapper*)malloc(sizeof(I2cBusLinuxWrapper));
    if (wrapper) {
        wrapper->i2cBusLinux = new I2cBusLinux(busId, name);
    }
    return wrapper;
}

void destroyI2cBusLinuxWrapper(I2cBusLinuxWrapper* instance) {
    if (instance) {
        delete instance->i2cBusLinux;
        free(instance);
    }
}

bool isI2cBusLinuxValid(I2cBusLinuxWrapper* instance) {
    return (instance && instance->i2cBusLinux && instance->i2cBusLinux->Valid());
}

void resetI2cBusLinux(I2cBusLinuxWrapper* instance) {
    if (isI2cBusLinuxValid(instance)) {
        instance->i2cBusLinux->reset();
    }
}

bool readDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount) {
    if (isI2cBusLinuxValid(instance)) {
        return instance->i2cBusLinux->readData(devAddr, data, byteCount) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

bool readByteI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data) {
    if (isI2cBusLinuxValid(instance)) {
        return instance->i2cBusLinux->readByte(devAddr, *data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

bool writeDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* data, unsigned int byteCount, unsigned int timeoutMsec) {
    if (isI2cBusLinuxValid(instance)) {
        return instance->i2cBusLinux->writeData(devAddr, data, byteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

bool writeByteI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char data) {
    if (isI2cBusLinuxValid(instance)) {
        return instance->i2cBusLinux->writeByte(devAddr, data) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}

bool writeReadDataI2cBusLinux(I2cBusLinuxWrapper* instance, unsigned char devAddr, unsigned char* command, unsigned int commandByteCount, unsigned char* data, unsigned int dataByteCount, unsigned int timeoutMsec) {
    if (isI2cBusLinuxValid(instance)) {
        return instance->i2cBusLinux->writeReadData(devAddr, command, commandByteCount, data, dataByteCount, timeoutMsec) == GenericDriverStatus::GenericDriverStatus_Success;
    }
    return false;
}
