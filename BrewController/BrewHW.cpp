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
  adc(&i2c, "WaterPressure", 0x48),
  pumpCTRL_PIN(rpiPinToBCM[37]),
  pump_ZC_SENSE_PIN(rpiPinToBCM[36]),
  steamSENSE_PIN(rpiPinToBCM[35]),
  brewSENSE_PIN(rpiPinToBCM[33]),
  valveRelayCTRL_PIN(rpiPinToBCM[31]),
  boilerRelayCTRL_PIN(rpiPinToBCM[29]),
  pump(rpiPinToBCM[36], rpiPinToBCM[37], POWER_LINE_FREQ, PUMP_FLOW_AT_ZERO) {

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

  gpioSetMode(valveRelayCTRL_PIN, PI_OUTPUT);
  gpioSetPullUpDown(valveRelayCTRL_PIN, PI_PUD_DOWN);
  gpioWrite(valveRelayCTRL_PIN, PI_LOW);

  gpioSetMode(boilerRelayCTRL_PIN, PI_OUTPUT);
  gpioSetPullUpDown(boilerRelayCTRL_PIN, PI_PUD_DOWN);
  gpioWrite(boilerRelayCTRL_PIN, PI_LOW);





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
    BLOG_ERROR("Boiler is fileld %u ms\n", elapsedTime);
    return true;
  }
  //return elapsedTime >= BOILER_FILL_TIMEOUT || boilerFull;
  return boilerFull;
}


// // Sample all sensors and return their readings
// SensorState BrewHW::getSensorState() {
//   //std::cout << "Sampling sensors..." << std::endl;
//   static int i = 0;
//   BrewDB& db = BrewDB::getInstance();
//   SensorState sensorState = {};
//   if (SIM)
//   {
//     return db.generateFakeSensorState(i);
//   }
//   else
//   {

//     //memset(&sensorState,0, sizeof(sensorState));
//     sensorState.iteration = i;
//     //sensorState.currentReadTime = std::chrono::system_clock::now();
//     sensorState.steamSwitchState = steamState();
//     sensorState.brewSwitchState = brewState();
//     sensorState.pressure = getPressure();
//     sensorState.temperature = getTemperature();
//   }
//   i++;

//   return sensorState;
// }

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
  BLOG_ERROR("Turning off pump and valve\n");
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
  digitalWrite(valveRelayCTRL_PIN, HIGH);
  //#endif
}

void BrewHW::closeValve(void) {
  // #if defined LEGO_VALVE_RELAY
  //   digitalWrite(valvePin, HIGH);
  // #else
  digitalWrite(valveRelayCTRL_PIN, LOW);
  // #endif
}


float BrewHW::getTemperature() {
  return tempSensor.readCelsius();
}

float BrewHW::getPressure() {

  float max_pressure_bar = 13.79;  // Maximum pressure in bar
  float min_voltage = 0.5;         // Sensor output at 0 pressure (in volts)
  float max_voltage = 4.5;         // Sensor output at max pressure (in volts)
  int16_t val_0 = adc.getValue();
  //printf("Analog%d: 0x%X  %f\n",i, val_0, adc.toVoltage(val_0) );
// Calculate pressure in bar
  float pressure = ((adc.toVoltage(val_0) - min_voltage) / (max_voltage - min_voltage)) * max_pressure_bar;


}
