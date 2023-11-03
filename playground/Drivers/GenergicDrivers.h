#pragma once
#include <mutex>
#include <stddef.h>
#define tmprintf(t, f, ...) // printf(...)

#define tmprintf_m(t, f, ...) // printf(...)
#define tmindent(t) //TmskIndent __tmindent__(t)
#define TMSK_spi 1
#define TMSK_i2c 1
#define byte uint8_t
enum GenericDriverStatus
{
  GenericDriverStatus_Success=0,
  GenericDriverStatus_NotImplemented=1,
  GenericDriverStatus_I2cError=2,
  GenericDriverStatus_I2cBusy=3,
  GenericDriverStatus_SpiError=4,
  GenericDriverStatus_Error_Argument_Invalid=5
};

void delay(long msec);
unsigned int millis () ;