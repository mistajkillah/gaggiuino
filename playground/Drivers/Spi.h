#pragma once
#include "GenergicDrivers.h"
class SpiBusAbstract
{
public:
  virtual ~SpiBusAbstract() {}
  virtual GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
    unsigned char* responseMessage, size_t numberReadBytes) = 0;

  virtual GenericDriverStatus reset(bool waitForReset = false) = 0;
};


class SpiBus : public SpiBusAbstract
{
  const char* __class__ = "SpiBus";

public:

  //typedef FpgaRegister::FpgaAddr FpgaAddr;
  //typedef FpgaRegister::FpgaData FpgaData;

  /*
   * Bus lock class.
   */
  class Lock : public std::unique_lock<std::recursive_mutex>
  {
    const char* __class__ = "SpiBus::Lock";

  public:
    Lock(SpiBus* bus);
    ~Lock();

  private:
    const SpiBus* bus;
  };

  SpiBus(const char* name) :
    _name(name) {
  }
  virtual ~SpiBus() {
  }



  virtual GenericDriverStatus sendReceiveBuffer(const unsigned char* sendMessage, size_t numberWriteBytes,
    unsigned char* responseMessage, size_t numberReadBytes) {
    (void)sendMessage;
    (void)numberWriteBytes;
    (void)responseMessage;
    (void)numberReadBytes;
    return GenericDriverStatus_NotImplemented;
  }

  virtual GenericDriverStatus reset(bool waitForReset = false) {
    (void)waitForReset;
    return GenericDriverStatus_NotImplemented;
  }

  static SpiBus& Instance() {
    return instance;
  }

private:

  GenericDriverStatus sendBuffer(const unsigned char* message, size_t sendByteCount, size_t numberReadBytes = 0) {
    return GenericDriverStatus_NotImplemented;
  }
  GenericDriverStatus readBuffer(unsigned char* message, size_t readByteCount, size_t writeByteCount) {
    return GenericDriverStatus_NotImplemented;
  }
  std::recursive_mutex mutex;
  static SpiBus instance;

protected:
  const char* _name;
};
