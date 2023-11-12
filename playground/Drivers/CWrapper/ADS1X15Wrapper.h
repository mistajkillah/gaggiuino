#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


// Forward declaration of the C wrapper struct
typedef struct ADS1X15Wrapper ADS1X15Wrapper;

// Forward declaration of the C wrapper struct for I2cBusLinuxWrapper
typedef struct I2cBusLinuxWrapper I2cBusLinuxWrapper;

// Function to create an instance of the ADS1X15Wrapper
ADS1X15Wrapper* createADS1X15Wrapper(I2cBusLinuxWrapper* bus, uint8_t devAddr, const char* name);

// Function to destroy the ADS1X15Wrapper instance
void destroyADS1X15Wrapper(ADS1X15Wrapper* instance);


// Function to destroy the ADS1X15Wrapper instance
void destroyADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to reset the ADS1X15Wrapper
void resetADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to initialize the ADS1X15Wrapper
bool beginADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to check if the ADS1X15Wrapper is connected
bool isConnectedADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the gain of the ADS1X15Wrapper
void setGainADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t gain);

// Function to get the gain of the ADS1X15Wrapper
uint8_t getGainADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the mode of the ADS1X15Wrapper
void setModeADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t mode);

// Function to get the mode of the ADS1X15Wrapper
uint8_t getModeADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the data rate of the ADS1X15Wrapper
void setDataRateADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t dataRate);

// Function to get the data rate of the ADS1X15Wrapper
uint8_t getDataRateADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to read ADC value from the ADS1X15Wrapper
int16_t readADCADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t pin);

// Function to read differential ADC value from the ADS1X15Wrapper
int16_t readADCDifferentialADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t positivePin, uint8_t negativePin);

// Function to request ADC value asynchronously from the ADS1X15Wrapper
void requestADCADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t pin);

// Function to check if the ADS1X15Wrapper is busy (asynchronous mode)
bool isBusyADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to check if the ADS1X15Wrapper is ready (asynchronous mode)
bool isReadyADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to get the last requested pin (asynchronous mode)
uint8_t lastRequestADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator mode of the ADS1X15Wrapper
void setComparatorModeADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t mode);

// Function to get the comparator mode of the ADS1X15Wrapper
uint8_t getComparatorModeADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator polarity of the ADS1X15Wrapper
void setComparatorPolarityADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t polarity);

// Function to get the comparator polarity of the ADS1X15Wrapper
uint8_t getComparatorPolarityADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator latch of the ADS1X15Wrapper
void setComparatorLatchADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t latch);

// Function to get the comparator latch of the ADS1X15Wrapper
uint8_t getComparatorLatchADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator queue conversion of the ADS1X15Wrapper
void setComparatorQueConvertADS1X15Wrapper(ADS1X15Wrapper* instance, uint8_t mode);

// Function to get the comparator queue conversion of the ADS1X15Wrapper
uint8_t getComparatorQueConvertADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator threshold low of the ADS1X15Wrapper
void setComparatorThresholdLowADS1X15Wrapper(ADS1X15Wrapper* instance, int16_t threshold);

// Function to get the comparator threshold low of the ADS1X15Wrapper
int16_t getComparatorThresholdLowADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to set the comparator threshold high of the ADS1X15Wrapper
void setComparatorThresholdHighADS1X15Wrapper(ADS1X15Wrapper* instance, int16_t threshold);

// Function to get the comparator threshold high of the ADS1X15Wrapper
int16_t getComparatorThresholdHighADS1X15Wrapper(ADS1X15Wrapper* instance);

// Function to get the error code of the ADS1X15Wrapper
int8_t getErrorADS1X15Wrapper(ADS1X15Wrapper* instance);

#ifdef __cplusplus
}
#endif
