#pragma once

#include <pigpio.h>
#include "Arduino.h"
#include "GenericDrivers.h"
#include "Spi.h"
#include "I2cBus.h"
#include "I2cBusLinux.h"
#include "ADS1X15.h"

#include "MAX6675_TempSensor.h"
#include "SensorStructs.h"
//class Measurments;

class BrewHW {
public:
    // Deleted copy constructor and assignment operator to prevent copying
    BrewHW(const BrewHW&) = delete;
    BrewHW& operator=(const BrewHW&) = delete;

    // Static method to access the singleton instance
    static BrewHW& getInstance();

    // Public methods
    int initializeHW();
    
    SensorState getSensorState();
    SensorStateSnapshot getSensorStateSnapshot();
    void resetHW();
    void cleanupHW();

    // GPIO pin constants
    static const int inputPin = 22;  // PIN 15 interrupt pin
    static const int outputPin = 27; // PIN 13 response to interrupt pin
    static const int togglePin = 17; // PIN 11 fake clock/interrupt aggressor

private:
    // Private constructor and destructor
    BrewHW();
    ~BrewHW();
    SensorState currentState;
    // Private methods to sample individual sensors
    std::string sampleSensor1();
    std::string sampleSensor2();

    // Static method for input callback
    static void inputCallback(int gpio, int level, uint32_t tick);
    void setBoilerOn(void);
    void setBoilerOff(void);
    void setSteamValveRelayOn(void);
    void setSteamValveRelayOff(void);
    void setSteamBoilerRelayOn(void);
    void setSteamBoilerRelayOff(void);
    int brewState(void);
    int steamState(void);
    int waterPinState(void);
    void openValve(void);
    void closeValve(void);
    void sensorReadSwitches(void);
    void sensorsReadTemperature(void);

    void sensorsReadPressure(void);

    int sensorsReadFlow(float elapsedTimeSec);

    void calculateWeightAndFlow(void);

    void sensorsReadWeight(void);

    float getTemperature();
    float getPressure();
    //scales vars
    //Measurements weightMeasurements;
    // Timers
    unsigned long systemHealthTimer=0;
    unsigned long pageRefreshTimer=0;
    unsigned long pressureTimer=0;
    unsigned long brewingTime=0;
    unsigned long thermoTimer=0;
    unsigned long scalesTimer=0;
    unsigned long flowTimer=0;
    unsigned long steamTim0;

    // Other util vars
    float previousSmoothedPressure;
    float previousSmoothedPumpFlow;

    // SimpleKalmanFilter &smoothPressure;
    // SimpleKalmanFilter &smoothPumpFlow;
    // SimpleKalmanFilter &smoothScalesFlow;
    // SimpleKalmanFilter &smoothConsideredFlow;

    // const int inputPin = 22;    //PIN 15 interupt pin
    // const int outputPin = 27;   //PIN 13 response to interupt pin
    // const int togglePin = 17;   //PIN 11 fake clock/interupt agressor
    SpiDeviceLinux  spiDevice;
    MAX6675_TempSensor tempSensor;
    I2cBusLinux i2c;
    ADS1015 adc;
};
