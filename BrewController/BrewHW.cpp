#include <iostream>
#include <cstring>
#include <thread>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cstring>
#include <sqlite3.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <random>
#include <cstdint>
#include <deque>

#include <pigpio.h>
#include "Arduino.h"
#include "SimpleKalmanFilter.h"
#include "measurements.h"
//#include "predictive_weight.h"
#include "Profile.h"
#include "BrewController.h"
#include "SensorStructs.h"

#include "GenericDrivers.h"
#include "Spi.h"
#include "I2cBus.h"
#include "I2cBusLinux.h"
#include "ADS1X15.h"

#include "MAX6675_TempSensor.h"

#include <pigpio.h>
#include <iostream>
#include <sqlite3.h>
#include "BrewDB.h"
#include "BrewHW.h"
//extern  BrewConfig runningCfg;

// Static method to access the singleton instance
BrewHW& BrewHW::getInstance() {
  static BrewHW instance;
  return instance;
}

// Private constructor
BrewHW::BrewHW() :
  //weightMeasurements(4), 
  // smoothPressure (*new SimpleKalmanFilter(0.6f, 0.6f, 0.1f)), 
  // smoothPumpFlow (*new SimpleKalmanFilter(0.1f, 0.1f, 0.01f)),
  // smoothScalesFlow (*new SimpleKalmanFilter(0.5f, 0.5f, 0.01f)),
  // smoothConsideredFlow (*new SimpleKalmanFilter(0.1f, 0.1f, 0.1f),
  spiDevice(0, 0, 4000000, 8, 1000, 0, "SpiDevice0.0"),
  tempSensor(&spiDevice, "BoilerTemp"),
  i2c(1, "i2c1"),
  adc(&i2c, "WaterTemp", 0x48),
  pump(pump_ZC_SENSE_PIN, pumpCTRL_PIN, POWER_LINE_FREQ, PUMP_FLOW_AT_ZERO) {



}

// Destructor
BrewHW::~BrewHW() {
  cleanupHW();
}

// Initialize hardware components
int BrewHW::initializeHW() {
  BLOG_DEBUG("Initializing hardware components...");

  // Initialize pigpio library
  if (gpioInitialise() < 0)
  {
    std::cerr << "Failed to initialize pigpio" << std::endl;
    return 1;
  }
  pump.Initialize();
  // Set GPIO modes
  //gpioSetMode(zc_Pin, PI_INPUT);
  //gpioSetMode(pumpCTRL_PIN, PI_OUTPUT);
  gpioSetMode(brewSENSE_PIN, PI_INPUT);
  gpioSetMode(steamSENSE_PIN, PI_INPUT);
  gpioSetPullUpDown(brewSENSE_PIN, PI_PUD_UP);
  gpioSetPullUpDown(steamSENSE_PIN, PI_PUD_UP);
  // Set pull-down resistor on the input pin
  //gpioSetPullUpDown(zc_Pin, PI_PUD_DOWN);

  // Set the callback function for the input pin
  //gpioSetAlertFunc(zc_Pin, inputCallback);


  adc.begin();
  adc.setGain(0);
  adc.setDataRate(4);  // fast
  adc.setMode(0);      // continuous mode
  adc.readADC(0);      // first read to trigger

  return 0;
}

// Sample all sensors and return their readings
SensorStateSnapshot BrewHW::getSensorStateSnapshot() {
  //std::cout << "Sampling sensors snapshot..." << std::endl;
  static int i = 0;
  BrewDB& db = BrewDB::getInstance();
  SensorStateSnapshot sensorStateSnapshot;
  if (SIM)
  {

    return db.generateFakeSensorStateSnapshot(i);
  }
  else
  {

  }
  i++;

  return sensorStateSnapshot;
}
float BrewHW::getPumpFlow(float elapsedTimeSec, long& currentPumpClicks, float& smoothedPressure) {
  // Retrieve and reset the pump click counter
  long pumpClicks = pump.getAndResetClickCounter();

  // Calculate pump clicks per second
  currentPumpClicks = static_cast<float>(pumpClicks) / elapsedTimeSec;

  // Calculate the pump flow based on current pump clicks and smoothed pressure
  return pump.getPumpFlow(currentPumpClicks, smoothedPressure);
}

bool BrewHW::isBoilerFull(SensorState& sensorState) {
  uint32_t elapsedTime = sensorState.getTimeSinceBrewStart();
  bool boilerFull = false;
  if (elapsedTime > BOILER_FILL_START_TIME + 1000UL)
  {
    boilerFull = (sensorState.previousSmoothedPressure - sensorState.smoothedPressure > -0.02f)
      &&
      (sensorState.previousSmoothedPressure - sensorState.smoothedPressure < 0.001f);
  }

  if (elapsedTime >= BOILER_FILL_TIMEOUT)
  {
    BLOG_ERROR("Boiler is fileld %u ms", elapsedTime);
    return true;
  }
  //return elapsedTime >= BOILER_FILL_TIMEOUT || boilerFull;
  return boilerFull;
}

void BrewHW::updateSensorStateAsync(SensorState& sensorState) {
  static int i = 0;
  std::future<float> pressureFuture;
  std::future<float> temperatureFuture;

  if (sensorState.timeSinceLastRead_pres_ms > GET_PRESSURE_READ_EVERY)
  {
    // Start parallel tasks using std::async
    pressureFuture = std::async(std::launch::async, &BrewHW::getPressure, this);
  }
  if (sensorState.timeSinceLastRead_temp_ms > GET_KTYPE_READ_EVERY)
  {
    // Start parallel tasks using std::async
    temperatureFuture = std::async(std::launch::async, &BrewHW::getTemperature, this);
  }

  //auto steamSwitchFuture = std::async(std::launch::async, &BrewHW::steamState, this);
  //auto brewSwitchFuture = std::async(std::launch::async, &BrewHW::brewState, this);

  sensorState.steamSwitchState = steamState();
  sensorState.brewSwitchState = brewState();
  // Set the iteration and timestamp
  sensorState.iteration = i;
  //sensorState.updateReadTime();
  // Wait for the results and assign them

  if (sensorState.timeSinceLastRead_pres_ms > GET_PRESSURE_READ_EVERY)
  {
    // Start parallel tasks using std::async
    sensorState.updatePressureReadTime();
    sensorState.pressure = pressureFuture.get();
    
  }
  if (sensorState.timeSinceLastRead_temp_ms > GET_KTYPE_READ_EVERY)
  {
    // Start parallel tasks using std::async
    sensorState.updateTemperatureReadTime();
    sensorState.temperature = temperatureFuture.get();
  }

  //sensorState.steamSwitchState = steamSwitchFuture.get();
  //sensorState.brewSwitchState = brewSwitchFuture.get();

  i++;
  //return sensorState;
}

// Sample all sensors and return their readings
SensorState BrewHW::getSensorState() {
  //std::cout << "Sampling sensors..." << std::endl;
  static int i = 0;
  BrewDB& db = BrewDB::getInstance();
  SensorState sensorState = {};
  if (SIM)
  {
    return db.generateFakeSensorState(i);
  }
  else
  {

    //memset(&sensorState,0, sizeof(sensorState));
    sensorState.iteration = i;
    //sensorState.currentReadTime = std::chrono::system_clock::now();
    sensorState.steamSwitchState = steamState();
    sensorState.brewSwitchState = brewState();
    sensorState.pressure = getPressure();
    sensorState.temperature = getTemperature();
  }
  i++;

  return sensorState;
}

// Reset hardware components to their default state
void BrewHW::resetHW() {
  std::cout << "Resetting hardware components..." << std::endl;
  // Add hardware reset logic here
}

// Clean up hardware resources
void BrewHW::cleanupHW() {
  std::cout << "Cleaning up hardware resources..." << std::endl;
  // Terminate pigpio library
  gpioTerminate();
}


//Function to get the state of the brew switch button
//returns true or false based on the read P(power) value
int BrewHW::brewState(void) {
  return digitalRead(brewSENSE_PIN) == LOW; // pin will be low when switch is ON.
}
void BrewHW::fillBoiler() {
  openValve();
  pump.setPumpToRawValue(35);
}

void BrewHW::stopFillBoiler() {
  closeValve();
  pump.setPumpOff();
}
// Returns HIGH when switch is OFF and LOW when ON
// pin will be high when switch is ON.
int BrewHW::steamState(void) {
  return digitalRead(steamSENSE_PIN) == LOW; // pin will be low when switch is ON.
}
// int BrewHW::waterPinState(void) {
//   #ifdef waterPin
//   return digitalRead(waterPin) == LOW; // pin will be low when switch is ON.
//   #else
//   return false;
//   #endif
// }
void BrewHW::openValve(void) {
  // #if defined LEGO_VALVE_RELAY
  //   digitalWrite(valvePin, LOW);
  // #else
  digitalWrite(valveCTRL_PIN, HIGH);
  //#endif
}

void BrewHW::closeValve(void) {
  // #if defined LEGO_VALVE_RELAY
  //   digitalWrite(valvePin, HIGH);
  // #else
  digitalWrite(valveCTRL_PIN, LOW);
  // #endif
}


float BrewHW::getTemperature() {
  return tempSensor.readCelsius();
}

float BrewHW::getPressure() {
  //int16_t val_0 = adc.readADC(0);  
  //int16_t val_1 = adc.readADC(1);  
  //int16_t val_2 = adc.readADC(2);  
  //int16_t val_3 = adc.readADC(3);  

  //float f = adc.toVoltage(1);  // voltage factor
  //printf("Analog0: 0x%X  %f\n", val_0, adc.toVoltage(val_0));
  //printf("Analog1: 0x%X  %f\n", val_1, adc.toVoltage(val_1));
  //printf("Analog2: 0x%X  %f\n", val_2, adc.toVoltage(val_2));
  //printf("Analog3: 0x%X  %f\n", val_3, adc.toVoltage(val_3));

   // previousPressure = currentPressure;
  return adc.getValue();//(adc.getValue() - 2666) / 1777.8f; // 16bit
}
