/*
 * I2cBusLinux.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: johnjackson
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <mutex>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "GenericDrivers.h"
#include "I2cBus.h"
#include "I2cBusLinux.h"


I2cBusLinux::~I2cBusLinux()
{
}


GenericDriverStatus I2cBusLinux::reset()
{
    return GenericDriverStatus_Success;
}


GenericDriverStatus I2cBusLinux::readData(unsigned char devAddr,
                                 unsigned char* data,
                                 unsigned int byteCount)
{
    GenericDriverStatus status = GenericDriverStatus_I2cError;
    int fd = -1;
    struct i2c_msg i2cMessage;
    struct i2c_rdwr_ioctl_data i2cIoctlData;

    LOG_MASK_I2C(
               "busId = %d, devAddr = 0x%02x, byteCount = %u\n",
               _busId,
               devAddr,
               byteCount);

    memset(data, 0, byteCount);
  
    Lock lock(this);

    if (!lock)
    {
        status = GenericDriverStatus_I2cBusy;

        goto end;
    }

    fd = i2c_bus_open(_busId);

    if (fd == -1)
    {
        LOG_MASK_ERROR( "open failed, bus = %d\n", _busId);

        goto end;
    }

#if 0
    /* set address */
    if (ioctl(busFD, I2C_SLAVE_FORCE, devAddr) == -1)
    {
        LOG_MASK_ERROR( "ioctl I2C_SLAVE_FORCE failed\n");
        goto finis;
    }
#endif

    i2cIoctlData.msgs  = &i2cMessage;
    i2cIoctlData.nmsgs = 1;

    i2cMessage.addr  = devAddr;
    i2cMessage.flags = I2C_M_RD;
    i2cMessage.len   = byteCount;
    i2cMessage.buf   = data;

    if (ioctl(fd, I2C_RDWR, &i2cIoctlData) == -1)
    {
        LOG_MASK_ERROR(
                   "ioctl failed, bus = %d, device = 0x%02x, errno = %d <%s>\n",
                   _busId,
                   devAddr,
                   errno,
                   strerror(errno));

        goto end;
    }

    status = GenericDriverStatus_Success;
    
end:

    i2c_bus_close(fd);

    LOG_MASK_I2C( "status = %s\n", GenericDriverStatus_str(status));

    return (status);
}

GenericDriverStatus I2cBusLinux::readByte(unsigned char devAddr, unsigned char& data)
{
    return readData(devAddr, &data, 1);
}


GenericDriverStatus I2cBusLinux::writeData(unsigned char devAddr,
                                  unsigned char* data, 
                                  unsigned int byteCount, 
                                  unsigned int timeoutMsec)
{
    GenericDriverStatus status = GenericDriverStatus_I2cError;
    int fd = -1;
    struct i2c_msg i2cMessage;
    struct i2c_rdwr_ioctl_data i2cIoctlData;

    LOG_MASK_I2C(
               "busId = %d, devAddr = 0x%2x, byteCount = %u, timeoutMsec = %u\n",
               _busId,
               devAddr,
               byteCount,
               timeoutMsec);

    Lock lock(this);

    if (!lock)
    {
        status = GenericDriverStatus_I2cBusy;

        goto end;
    }

    fd = i2c_bus_open(_busId);

    if (fd == -1)
    {
        LOG_MASK_ERROR( "open failed, bus = %d\n", _busId);

        goto end;
    }

#if 0
    /* set address */
    if (ioctl(busFD, I2C_SLAVE_FORCE, devAddr) < 0) {
        LOG_MASK_ERROR("ioctl I2C_SLAVE_FORCE failed\n");
        goto finis;
    }
#endif

    i2cIoctlData.msgs  = &i2cMessage;
    i2cIoctlData.nmsgs = 1;

    i2cMessage.addr = devAddr;
    i2cMessage.flags = 0;
    i2cMessage.len = byteCount;
    i2cMessage.buf = data;
       
    if (ioctl(fd, I2C_RDWR, &i2cIoctlData) == -1)
    {
        LOG_MASK_ERROR(
                   "ioctl failed, bus = %d, device = 0x%02x, errno = %d <%s>\n",
                   _busId,
                   devAddr,
                   errno,
                   strerror(errno));

        goto end;
    }

    status = GenericDriverStatus_Success;

end:

    i2c_bus_close(fd);

    LOG_MASK_I2C( "status = %s\n", GenericDriverStatus_str(status));

    return (status);
}


GenericDriverStatus I2cBusLinux::writeByte(unsigned char devAddr, unsigned char data)
{
    return writeData(devAddr, &data, 1);
}


GenericDriverStatus I2cBusLinux::writeReadData(unsigned char devAddr,
                                      unsigned char* command,
                                      unsigned int commandByteCount,
                                      unsigned char* data,
                                      unsigned int dataByteCount,
                                      unsigned int timeoutMsec)
{
    GenericDriverStatus status = GenericDriverStatus_I2cError;
    int fd = -1;
    struct i2c_msg i2cMessages[2];
    struct i2c_rdwr_ioctl_data i2cIoctlData;

    LOG_MASK_I2C(
               "busId = %d, devAddr = 0x%02x, commandByteCount = %u, dataByteCount = %u\n",
               _busId,
               devAddr,
               commandByteCount,
               dataByteCount);

    Lock lock(this);

    if (!lock)
    {
        status = GenericDriverStatus_I2cBusy;

        goto end;
    }

    if ((command == nullptr) || (data == nullptr))
    {
        LOG_MASK_ERROR( "null pointer\n");

        goto end;
    }

    if ((commandByteCount == 0) || (dataByteCount == 0))
    {
        LOG_MASK_ERROR(
            "invalid byte count, commandByteCount = %u, dataByteCount = %u\n",
            commandByteCount,
            dataByteCount);

        goto end;
    }

    memset(data, 0, dataByteCount);

    fd = i2c_bus_open(_busId);

    if (fd == -1)
    {
        LOG_MASK_ERROR( "open failed, bus = %d\n", _busId);

        goto end;
    }

    i2cIoctlData.msgs  = &i2cMessages[0];
    i2cIoctlData.nmsgs = 2;

    i2cMessages[0].addr  = devAddr;
    i2cMessages[0].flags = 0;
    i2cMessages[0].len   = commandByteCount;
    i2cMessages[0].buf   = command;

    i2cMessages[1].addr  = devAddr;
    i2cMessages[1].flags = I2C_M_RD;
    i2cMessages[1].len   = dataByteCount;
    i2cMessages[1].buf   = data;

    if (ioctl(fd, I2C_RDWR, &i2cIoctlData) == -1)
    {
        LOG_MASK_ERROR(
                   "ioctl failed, bus = %d, device = 0x%02x, errno = %d <%s>\n",
                   _busId,
                   devAddr,
                   errno,
                   strerror(errno));

        goto end;
    }

    status = GenericDriverStatus_Success;

end:

    i2c_bus_close(fd);

    LOG_MASK_I2C( "status = %s\n", GenericDriverStatus_str(status));

    return (status);
}


int I2cBusLinux::i2c_bus_open(int busId)
{
    char wbuf[32];
    int busFD = -1;

    sprintf(wbuf, "/dev/i2c-%d", busId);

    busFD = open(wbuf, O_RDWR);
    if (busFD < 0) {
        LOG_MASK_ERROR( "can't open %s, %d\n", wbuf, errno);
    }

    /* done */
    return busFD;
}


void  I2cBusLinux::i2c_bus_close(int busFD)
{
    if (busFD >= 0) {
        if (0 != close(busFD))
        {
            LOG_MASK_ERROR( "can't close bus %d,  %d\n", busFD, errno);
        }
        busFD = -1;
    }
    return;
}
