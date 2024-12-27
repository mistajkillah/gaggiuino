#pragma once

#include <pigpio.h>
#include "Arduino.h"
#include "GenericDrivers.h"
#include "Spi.h"
#include "I2cBus.h"
#include "I2cBusLinux.h"
#include "ADS1X15.h"
#include "Pump.h"
#include "MAX6675_TempSensor.h"
#include "SensorStructs.h"
//class Measurments;

class BrewHW
{
public:
  // Deleted copy constructor and assignment operator to prevent copying
  BrewHW(const BrewHW&) = delete;
  BrewHW& operator=(const BrewHW&) = delete;

  // Static method to access the singleton instance
  static BrewHW& getInstance();

  // Public methods
  int initializeHW();

  SensorState getSensorState();
  SensorState updateSensorStateAsync();
  SensorStateSnapshot getSensorStateSnapshot();
  float getPumpFlow(float elapsedTimeSec, long& currentPumpClicks, float& smoothedPressure);

  bool isBoilerFull(SensorState& sensorState);
  void updateSensorStateAsync(SensorState& sensorState);
  void resetHW();
  void cleanupHW();
  void fillBoiler();

  void stopFillBoiler();

  float getTemperature();
  float getPressure();
  int brewState(void);

  int steamState(void);
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

  int waterPinState(void);
  void openValve(void);
  void closeValve(void);
  void sensorReadSwitches(void);
  void sensorsReadTemperature(void);

  void sensorsReadPressure(void);



  void calculateWeightAndFlow(void);

  void sensorsReadWeight(void);


  void fillBoilerUntilThreshod(unsigned long elapsedTime);
  //scales vars
  //Measurements weightMeasurements;
  // Timers
  unsigned long systemHealthTimer = 0;
  unsigned long pageRefreshTimer = 0;
  unsigned long pressureTimer = 0;
  unsigned long brewingTime = 0;
  unsigned long thermoTimer = 0;
  unsigned long scalesTimer = 0;
  unsigned long flowTimer = 0;
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
  Pump pump;

  // Array to map physical pin to BCM GPIO pin
  int rpiPinToBCM[41] = {
      -1,  // Index 0 (no pin 0, placeholder for 1-based indexing)
      -1,  // Pin 1: 3.3V
      -1,  // Pin 2: 5V
       2,  // Pin 3: GPIO2 (SDA1)
      -1,  // Pin 4: 5V
       3,  // Pin 5: GPIO3 (SCL1)
      -1,  // Pin 6: Ground
       4,  // Pin 7: GPIO4
      14,  // Pin 8: GPIO14 (TXD)
      -1,  // Pin 9: Ground
      15,  // Pin 10: GPIO15 (RXD)
      17,  // Pin 11: GPIO17
      18,  // Pin 12: GPIO18
      27,  // Pin 13: GPIO27
      -1,  // Pin 14: Ground
      22,  // Pin 15: GPIO22
      23,  // Pin 16: GPIO23
      -1,  // Pin 17: 3.3V
      24,  // Pin 18: GPIO24
      10,  // Pin 19: GPIO10 (MOSI)
      -1,  // Pin 20: Ground
       9,  // Pin 21: GPIO9 (MISO)
      25,  // Pin 22: GPIO25
      11,  // Pin 23: GPIO11 (SCLK)
       8,  // Pin 24: GPIO8 (CE0)
      -1,  // Pin 25: Ground
       7,  // Pin 26: GPIO7 (CE1)
      -1,  // Pin 27: ID_SD
      -1,  // Pin 28: ID_SC
       5,  // Pin 29: GPIO5
      -1,  // Pin 30: Ground
       6,  // Pin 31: GPIO6
      12,  // Pin 32: GPIO12
      13,  // Pin 33: GPIO13
      -1,  // Pin 34: Ground
      19,  // Pin 35: GPIO19
      16,  // Pin 36: GPIO16
      26,  // Pin 37: GPIO26
      20,  // Pin 38: GPIO20
      -1,  // Pin 39: Ground
      21   // Pin 40: GPIO21
  };

  const int pumpCTRL_PIN = rpiPinToBCM[37];//YELLOW  raspi-gpio set 26 op pd dl 

  const int pump_ZC_SENSE_PIN = rpiPinToBCM[36];//Purple  BMC 16 GPIO 27 

  const int steamSENSE_PIN = rpiPinToBCM[35];//R ED 12 Pulled UP  broke
  const int brewSENSE_PIN = rpiPinToBCM[33];//BROWN 13 Pulled UP;  

  const int valveRelayCTRL_PIN = rpiPinToBCM[31];//RED BCM 6 GPIO 22  pulled down output dl raspi-gpio set 6 op pd dl

  const int boilerRelayCTRL_PIN = rpiPinToBCM[29];//FAT YELLOW BCM 5 GPIO 21  pulled down output dl  raspi-gpio set 5 op pd dl 


  const int POWER_LINE_FREQ = 60;
  const float PUMP_FLOW_AT_ZERO = 0.2225f;

};
