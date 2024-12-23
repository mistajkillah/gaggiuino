#include <mutex>
#include "I2cBus.h"
#ifdef __cplusplus
extern "C" {
#endif

//I2cBus I2cBus::s_empty("Empty");


I2cBus::I2cBus(const char* name) :
    _name(name)
{
}


I2cBus::Lock::Lock(I2cBus *bus) :
    std::unique_lock<std::recursive_mutex>(bus->mutex, std::defer_lock),
    bus(bus)
{
    try_lock();
    
    LOG_MASK_I2C( "bus=%s, acquired=%d\n", bus->_name,  owns_lock());
}



I2cBus::Lock::~Lock()
{
    
    LOG_MASK_I2C( "bus=%s\n", bus->_name);
}
#ifdef __cplusplus
}
#endif