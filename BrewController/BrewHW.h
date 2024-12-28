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


  SpiDeviceLinux  spiDevice;
  MAX6675_TempSensor tempSensor;
  I2cBusLinux i2c;
  ADS1015 adc;



  // // const int pumpCTRL_PIN = rpiPinToBCM[37];//YELLOW  raspi-gpio set 26 op pd dl 

  // // const int pump_ZC_SENSE_PIN = rpiPinToBCM[36];//Purple  BMC 16 GPIO 27 

  // // const int steamSENSE_PIN = rpiPinToBCM[35];//R ED 12 Pulled UP  broke
  // // const int brewSENSE_PIN = rpiPinToBCM[33];//BROWN 13 Pulled UP;  

  // // const int valveRelayCTRL_PIN = rpiPinToBCM[31];//RED BCM 6 GPIO 22  pulled down output dl raspi-gpio set 6 op pd dl

  // // const int boilerRelayCTRL_PIN = rpiPinToBCM[29];//FAT YELLOW BCM 5 GPIO 21  pulled down output dl  raspi-gpio set 5 op pd dl 
  const int pumpCTRL_PIN;
  const int pump_ZC_SENSE_PIN;
  const int steamSENSE_PIN;
  const int brewSENSE_PIN;
  const int valveRelayCTRL_PIN;
  const int boilerRelayCTRL_PIN;

  const int POWER_LINE_FREQ = 60;
  const float PUMP_FLOW_AT_ZERO = 0.2225f;
  Pump pump;
};