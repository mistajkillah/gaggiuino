/*
 * Spi.cpp
 *
 * SPI Byte / Bit order:
 *   Note SPI sends data MSB first.  So we represent data as a byte string with
 *   the first byte (or leftmost byte looking at it as a string) is shifted first
 *   MSB of each byte first as well.  In other words, this is BIG ENDIAN.
 *
 *   For transactions that are other than a multiple of 8 bits, the last byte in the
 *   byte array is truncated as needed.
 *
 *  Created on: Aug 18, 2016
 *      Author: reichert
 */


 /* Endianess.
  *   myUnion.asByte[0] == 0x03 ? "BIG" : "LITTLE",
  *   For Big Endian machines (PPC), the 0th byte of an integer is (or a 4 byte byte array!)
  *   is the Most Significant byte, which is what we want.
  *
  *   For little endian machines (Intel, Arm) the 0th byte of the byte array is
  *   the least significant byte of an integer, so we need to do a byte swap.
  *   Note that this is the opposite of when we typically do byte swapping due to the
  *   oddity of running on a PPC.
  */

#include <assert.h>
#include <ctype.h>
#include <sys/time.h>
#include <string>
#include "Spi.h"


Spi::Lock::Lock(Spi* bus) :
    std::unique_lock<std::recursive_mutex>(bus->mutex, std::defer_lock),
    bus(bus)
{

    lock(); //try_lock() //todo
    tmindent(TMSK_spi);
    tmprintf_m(TMSK_spi, "bus=%s, acquired=%d\n", bus->name, owns_lock());
}


/*
 * Lock destructor. Releases the bus's mutex, unless another Lock object in the same
 * thread is still holding it.
 *
 */
Spi::Lock::~Lock()
{
    tmindent(TMSK_spi);
    tmprintf_m(TMSK_spi, "bus=%s\n", bus->name);
}
