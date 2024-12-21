#pragma once
#include <mutex>
#include <stddef.h>
#define LOG_MASK_ERROR(format,...) fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__)
#define LOG_MASK_SPI(format,...) fprintf(stderr,format __VA_OPT__(,) __VA_ARGS__)
#define LOG_MASK_I2C(format,...) fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__)
#define GenericDriverStatus_str(x) "empty"
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

