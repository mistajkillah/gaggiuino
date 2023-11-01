#include "I2cBus.h"


I2cBus I2cBus::s_empty("Empty");


I2cBus::I2cBus(const char* name) :
    _name(name)
{
}


/*
 * Lock constructor. Attempts to acquires the bus's mutex, returning immediately.
 * The lock will be acquired if it is not currently held or is held by the same thread.
 * An explicit cast to bool will return true if the lock was acquired, false if not.
 *
 * returns - An I2cBus::Lock object.
 */
I2cBus::Lock::Lock(I2cBus *bus) :
    std::unique_lock<std::recursive_mutex>(bus->mutex, std::defer_lock),
    bus(bus)
{
    try_lock();
    tmindent(TMSK_i2c);
    tmprintf_m(TMSK_i2c, "bus=%s, acquired=%d\n", bus->_name,  owns_lock());
}


/*
 * Lock destructor. Releases the bus's mutex, unless another Lock object in the same
 * thread is still holding it.
 *
 */
I2cBus::Lock::~Lock()
{
    tmindent(TMSK_i2c);
    tmprintf_m(TMSK_i2c, "bus=%s\n", bus->_name);
}
