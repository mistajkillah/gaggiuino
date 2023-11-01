#pragma once
#include "GenergicDrivers.h"
class SpiAbstract
{
public:
	virtual ~SpiAbstract() { }
    virtual GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes) = 0;

    virtual GenericDriverStatus reset(bool waitForReset = false) = 0;
};


class Spi : public SpiAbstract
{
    const char* __class__ = "Spi";

public:

    //typedef FpgaRegister::FpgaAddr FpgaAddr;
    //typedef FpgaRegister::FpgaData FpgaData;

    /*
     * Bus lock class.
     */
    class Lock : public std::unique_lock<std::recursive_mutex>
    {
        const char* __class__ = "Spi::Lock";

    public:
        Lock(Spi* bus);
        ~Lock();

    private:
        const Spi* bus;
    };

    Spi(const char* name):
    _name(name)
    {
    }
    virtual ~Spi()
    {}



  GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
        unsigned char* responseMessage, size_t numberReadBytes)
    {
        (void)sendMessage;
        (void)numberWriteBytes;
        (void)responseMessage;
        (void)numberReadBytes;
        return GenericDriverStatus_NotImplemented;
    }

    GenericDriverStatus reset(bool waitForReset = false)
    {
        (void)waitForReset;
        return GenericDriverStatus_NotImplemented;
    }

    static Spi &Instance()
    {
    	return instance;
    }

private:

    GenericDriverStatus sendBuffer(const unsigned char* message, size_t sendByteCount, size_t numberReadBytes = 0)
    {
      return GenericDriverStatus_NotImplemented;
    }
    GenericDriverStatus readBuffer(unsigned char* message, size_t readByteCount, size_t writeByteCount)
    {
      return GenericDriverStatus_NotImplemented;
    }
    std::recursive_mutex mutex;
    static Spi instance;

protected:
const char *_name;
};

