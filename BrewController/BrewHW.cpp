#include <iostream>
#include <cstring>
#include <thread>

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

#include <pigpio.h>
#include "Arduino.h"
#include "BrewController.h"
#include "SensorStructs.h"
#include "BrewDB.h"
#include "BrewHW.h"




// Static method to access the singleton instance
BrewHW& BrewHW::getInstance() {
    static BrewHW instance;
    return instance;
}

// Private constructor
BrewHW::BrewHW() {
    // Initialization code, if necessary
}

// Destructor
BrewHW::~BrewHW() {
    cleanupHW();
}

// Initialize hardware components
int BrewHW::initializeHW() {
    std::cout << "Initializing hardware components..." << std::endl;

    // Initialize pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio" << std::endl;
        return 1;
    }

    // Set GPIO modes
    gpioSetMode(inputPin, PI_INPUT);
    gpioSetMode(outputPin, PI_OUTPUT);
    gpioSetMode(togglePin, PI_OUTPUT);

    // Set pull-down resistor on the input pin
    gpioSetPullUpDown(inputPin, PI_PUD_DOWN);

    // Set the callback function for the input pin
    gpioSetAlertFunc(inputPin, inputCallback);

    return 0;
}

// Sample all sensors and return their readings
SensorStateSnapshot BrewHW::getSensorStateSnapshot() {
    std::cout << "Sampling sensors snapshot..." << std::endl;
    static int i = 0;
    BrewDB &db = BrewDB::getInstance();
    SensorStateSnapshot sensorStateSnapshot;
    if(SIM)
    {

        return db.generateFakeSensorStateSnapshot(i);
    }
    else{
        
    }
    i++;

    return sensorStateSnapshot;
}


// Sample all sensors and return their readings
SensorState BrewHW::getSensorState() {
    std::cout << "Sampling sensors..." << std::endl;
    static int i = 0;
    BrewDB &db = BrewDB::getInstance();
    SensorState sensorState;
    if(SIM)
    {

        return db.generateFakeSensorState(i);
    }
    else{
        
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

// Private methods to sample individual sensors
std::string BrewHW::sampleSensor1() {
    std::cout << "Sampling sensor 1..." << std::endl;
    return "Sensor1_Data";
}

std::string BrewHW::sampleSensor2() {
    std::cout << "Sampling sensor 2..." << std::endl;
    return "Sensor2_Data";
}

// Static method for input callback
void BrewHW::inputCallback(int gpio, int level, uint32_t tick) {
    // Set the output pin based on the input pin level
    gpioWrite(outputPin, level);
}


// Actuating the heater element
void BrewHW::setBoilerOn(void) {
  digitalWrite(relayPin, HIGH);  // boilerPin -> HIGH
}

void BrewHW::setBoilerOff(void) {
  digitalWrite(relayPin, LOW);  // boilerPin -> LOW
}

void BrewHW::setSteamValveRelayOn(void) {
  #ifdef steamValveRelayPin
  digitalWrite(steamValveRelayPin, HIGH);  // steamValveRelayPin -> HIGH
  #endif
}

void BrewHW::setSteamValveRelayOff(void) {
  #ifdef steamValveRelayPin
  digitalWrite(steamValveRelayPin, LOW);  // steamValveRelayPin -> LOW
  #endif
}

void BrewHW::setSteamBoilerRelayOn(void) {
  #ifdef steamBoilerRelayPin
  digitalWrite(steamBoilerRelayPin, HIGH);  // steamBoilerRelayPin -> HIGH
  #endif
}

void BrewHW::setSteamBoilerRelayOff(void) {
  #ifdef steamBoilerRelayPin
  digitalWrite(steamBoilerRelayPin, LOW);  // steamBoilerRelayPin -> LOW
  #endif
}

//Function to get the state of the brew switch button
//returns true or false based on the read P(power) value
int BrewHW::brewState(void) {
  return digitalRead(brewPin) == LOW; // pin will be low when switch is ON.
}

// Returns HIGH when switch is OFF and LOW when ON
// pin will be high when switch is ON.
int BrewHW::steamState(void) {
  return digitalRead(steamPin) == LOW; // pin will be low when switch is ON.
}
int BrewHW::waterPinState(void) {
  #ifdef waterPin
  return digitalRead(waterPin) == LOW; // pin will be low when switch is ON.
  #else
  return false;
  #endif
}
void BrewHW::openValve(void) {
  #if defined LEGO_VALVE_RELAY
    digitalWrite(valvePin, LOW);
  #else
    digitalWrite(valvePin, HIGH);
  #endif
}

void BrewHW::closeValve(void) {
  #if defined LEGO_VALVE_RELAY
    digitalWrite(valvePin, HIGH);
  #else
    digitalWrite(valvePin, LOW);
  #endif
}



void BrewHW::sensorReadSwitches(void) {
  currentState.brewSwitchState = brewState();
  currentState.steamSwitchState = steamState();
  currentState.hotWaterSwitchState = waterPinState() || (currentState.brewSwitchState && currentState.steamSwitchState); // use either an actual switch, or the GC/GCP switch combo
}

void BrewHW::sensorsReadTemperature(void) {
  if (millis() > thermoTimer) {
    currentState.temperature = thermocoupleRead() - runningCfg.offsetTemp;
    thermoTimer = millis() + GET_KTYPE_READ_EVERY;
  }
}

// static void sensorsReadWeight(void) {
//   uint32_t elapsedTime = millis() - scalesTimer;

//   if (elapsedTime > GET_SCALES_READ_EVERY) {
//     //currentState.scalesPresent = scalesIsPresent();
//     if (currentState.scalesPresent) {
//       if (currentState.tarePending) {
//         //scalesTare();
//         weightMeasurements.clear();
//        // weightMeasurements.add(scalesGetWeight());
//         currentState.tarePending = false;
//       }
//       else {
//         //weightMeasurements.add(scalesGetWeight());
//       }
//       currentState.weight = weightMeasurements.latest().value;

//       if (brewActive) {
//         currentState.shotWeight = currentState.tarePending ? 0.f : currentState.weight;
//         currentState.weightFlow = fmax(0.f, weightMeasurements.measurementChange().changeSpeed());
//         currentState.smoothedWeightFlow = smoothScalesFlow.updateEstimate(currentState.weightFlow);
//       }
//     }
//     scalesTimer = millis();
//   }
// }

// static void sensorsReadPressure(void) {
//   uint32_t elapsedTime = millis() - pressureTimer;

//   if (elapsedTime > GET_PRESSURE_READ_EVERY) {
//     float elapsedTimeSec = elapsedTime / 1000.f;
//     currentState.pressure = getPressure();
//     previousSmoothedPressure = currentState.smoothedPressure;
//     currentState.smoothedPressure = smoothPressure.updateEstimate(currentState.pressure);
//     currentState.pressureChangeSpeed = (currentState.smoothedPressure - previousSmoothedPressure) / elapsedTimeSec;
//     pressureTimer = millis();
//   }
// }

// static long sensorsReadFlow(float elapsedTimeSec) {
//   long pumpClicks = getAndResetClickCounter();
//   currentState.pumpClicks = (float) pumpClicks / elapsedTimeSec;

//   currentState.pumpFlow = getPumpFlow(currentState.pumpClicks, currentState.smoothedPressure);

//   previousSmoothedPumpFlow = currentState.smoothedPumpFlow;
//   // Some flow smoothing
//   currentState.smoothedPumpFlow = smoothPumpFlow.updateEstimate(currentState.pumpFlow);
//   currentState.pumpFlowChangeSpeed = (currentState.smoothedPumpFlow - previousSmoothedPumpFlow) / elapsedTimeSec;
//   return pumpClicks;
// }

// static void calculateWeightAndFlow(void) {
//   uint32_t elapsedTime = millis() - flowTimer;

//   if (brewActive) {
//     // Marking for tare in case smth has gone wrong and it has exited tare already.
//     if (currentState.weight < -.3f) currentState.tarePending = true;

//     if (elapsedTime > REFRESH_FLOW_EVERY) {
//       flowTimer = millis();
//       float elapsedTimeSec = elapsedTime / 1000.f;
//       long pumpClicks = sensorsReadFlow(elapsedTimeSec);
//       float consideredFlow = currentState.smoothedPumpFlow * elapsedTimeSec;
//       // Update predictive class with our current phase
//       CurrentPhase& phase = phaseProfiler.getCurrentPhase();
//       predictiveWeight.update(currentState, phase, runningCfg);

//       // Start the predictive weight calculations when conditions are true
//       if (predictiveWeight.isOutputFlow() || currentState.weight > 0.4f) {
//         float flowPerClick = getPumpFlowPerClick(currentState.smoothedPressure);
//         float actualFlow = (consideredFlow > pumpClicks * flowPerClick) ? consideredFlow : pumpClicks * flowPerClick;
//         /* Probabilistically the flow is lower if the shot is just started winding up and we're flow profiling,
//         once pressure stabilises around the setpoint the flow is either stable or puck restriction is high af. */
//         if ((ACTIVE_PROFILE(runningCfg).mfProfileState || ACTIVE_PROFILE(runningCfg).tpType) && currentState.pressureChangeSpeed > 0.15f) {
//           if ((currentState.smoothedPressure < ACTIVE_PROFILE(runningCfg).mfProfileStart * 0.9f)
//           || (currentState.smoothedPressure < ACTIVE_PROFILE(runningCfg).tfProfileStart * 0.9f)) {
//             actualFlow *= 0.3f;
//           }
//         }
//         currentState.consideredFlow = smoothConsideredFlow.updateEstimate(actualFlow);
//         currentState.shotWeight = currentState.scalesPresent ? currentState.shotWeight : currentState.shotWeight + actualFlow;
//       }
//       currentState.waterPumped += consideredFlow;
//     }
//   } else {
//     currentState.consideredFlow = 0.f;
//     currentState.pumpClicks = getAndResetClickCounter();
//     flowTimer = millis();
//   }
// }