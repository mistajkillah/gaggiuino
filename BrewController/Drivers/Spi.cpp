

#include <assert.h>
#include <ctype.h>
#include <sys/time.h>
#include <string>
#include "Spi.h"


SpiBus::Lock::Lock(SpiBus* bus) :
  std::unique_lock<std::recursive_mutex>(bus->mutex, std::defer_lock),
  bus(bus) {

  lock(); //try_lock() //todo
  
  LOG_MASK_SPI( "bus=%s, acquired=%d\n", bus->_name, owns_lock());
}


/*
 * Lock destructor. Releases the bus's mutex, unless another Lock object in the same
 * thread is still holding it.
 *
 */
SpiBus::Lock::~Lock() {
  
  LOG_MASK_SPI( "bus=%s\n", bus->_name);
}
