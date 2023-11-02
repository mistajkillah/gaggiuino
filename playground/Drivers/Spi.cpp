

#include <assert.h>
#include <ctype.h>
#include <sys/time.h>
#include <string>
#include "Spi.h"


SpiBus::Lock::Lock(SpiBus* bus) :
  std::unique_lock<std::recursive_mutex>(bus->mutex, std::defer_lock),
  bus(bus) {

  lock(); //try_lock() //todo
  tmindent(TMSK_spi);
  tmprintf_m(TMSK_spi, "bus=%s, acquired=%d\n", bus->name, owns_lock());
}


/*
 * Lock destructor. Releases the bus's mutex, unless another Lock object in the same
 * thread is still holding it.
 *
 */
SpiBus::Lock::~Lock() {
  tmindent(TMSK_spi);
  tmprintf_m(TMSK_spi, "bus=%s\n", bus->name);
}
