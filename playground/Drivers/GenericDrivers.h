#pragma once
#include <mutex>
#include <stddef.h>
#include <stdint.h> // for uint8_t
#include <stdio.h>

// Define the logging level. Uncomment the desired level to enable logging.
// Levels: ALL, I2C, SPI, DEBUG, INFO, ERROR
//#define LOG_LEVEL_ALL
//#define LOG_LEVEL_I2C
//#define LOG_LEVEL_SPI
//#define LOG_LEVEL_DEBUG
//#define LOG_LEVEL_INFO
//#define LOG_LEVEL_ERROR

// Logging levels
#define LOG_LEVEL_ALL 0
#define LOG_LEVEL_I2C 1
#define LOG_LEVEL_SPI 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_ERROR 5

// Set the active logging level here
#ifndef ACTIVE_LOG_LEVEL
#define ACTIVE_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// Unified logging function
#define LOG(level, format, ...) \
    do { \
        if (level >= ACTIVE_LOG_LEVEL) { \
            fprintf(stderr, format, ##__VA_ARGS__); \
        } \
    } while (0)

// Convenience macros for specific log levels
#define LOG_ERROR(format, ...) LOG(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define LOG_SPI(format, ...) LOG(LOG_LEVEL_SPI, format, ##__VA_ARGS__)
#define LOG_I2C(format, ...) LOG(LOG_LEVEL_I2C, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG(LOG_LEVEL_INFO, format, ##__VA_ARGS__)

// Status-based logging macro
#define LOG_STATUS(status, format, ...) \
    do { \
        if ((status) != 0) { \
            fprintf(stderr, format, ##__VA_ARGS__); \
        } \
    } while (0)

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

