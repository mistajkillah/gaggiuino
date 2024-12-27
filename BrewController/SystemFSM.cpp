#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <sstream>
#include <sqlite3.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <random>
#include <deque>
#include <assert.h>
#include "BrewController.h"
#include "SensorStructs.h"
#include "SimpleKalmanFilter.h"
#include "measurements.h"

#include "SystemFSM.h"
#include "BrewHW.h"
#include "BrewDB.h"
#include <future>


// Constructor
SystemFSM::SystemFSM() : hw(BrewHW::getInstance()){}

void SystemFSM::handleInitialize() {
  BLOG_INFO("System is initializing...\n");
  system("rm -rf /tmp/mydb.sqlite");
  BrewDB& brewDB = BrewDB::getInstance();
  brewDB.Initialize();
  hw.initializeHW();
  sensorState.setTimeSinceSystemStart();
  currentState = SystemState::SystemIdeling;
}

void SystemFSM::updateSensorStateAsync() {
    static int i = 0;
    std::future<float> pressureFuture;
    std::future<float> temperatureFuture;

    // Check and start async tasks based on conditions
    if (sensorState.getTimeSincelastRead_pres() > GET_PRESSURE_READ_EVERY) {
        // Use a lambda to capture 'hw' explicitly
        pressureFuture = std::async(std::launch::async, [this]() {
            return hw.getPressure();
        });
    }
    if (sensorState.getTimeSincelastRead_temp() > GET_KTYPE_READ_EVERY) {
        // Use a lambda to capture 'hw' explicitly
        temperatureFuture = std::async(std::launch::async, [this]() {
            return hw.getTemperature();
        });
    }

    // Update switch states directly
    sensorState.steamSwitchState = hw.steamState();
    sensorState.brewSwitchState = hw.brewState();

    // Increment iteration
    sensorState.iteration = i;

    // Wait for temperature and update state
    if (sensorState.getTimeSincelastRead_temp() > GET_KTYPE_READ_EVERY) {
        sensorState.updateTemperatureReadTime();
        if (temperatureFuture.valid()) {
            sensorState.temperature = temperatureFuture.get();
        }
    }

    // Wait for pressure and update state
    if (sensorState.getTimeSincelastRead_pres() > GET_PRESSURE_READ_EVERY) {
        sensorState.updatePressureReadTime();
        if (pressureFuture.valid()) {
            sensorState.pressure = pressureFuture.get();
        }
        updatePressue(); // Additional processing for pressure
    }

    i++;
}

SystemState SystemFSM::handleSystemIdleing() {

  updateSensorStateAsync();
  //hw.updateSensorStateAsync(sensorState);
  //updatePressue();
  BLOG_ERROR("%s\n", sensorState.toString().c_str());
  if (false == sensorState.brewSwitchState)
  {
    
    BLOG_ERROR("BREW switched off...\n");
    currentState = SystemState::SystemIdeling; 
  
  }
  else
  {
    currentState = SystemState::Brewing;
  }
  return currentState;
}



void SystemFSM::updatePressue(void) {  
  //uint32_t elapsedTime = millis() - pressureTimer;
  if (sensorState.timeSinceLastRead_pres_ms > GET_PRESSURE_READ_EVERY) {
    

    sensorState.previousSmoothedPressure = sensorState.smoothedPressure;
    sensorState.smoothedPressure = smoothPressure.updateEstimate(sensorState.pressure);
    sensorState.pressureChangeSpeed = (sensorState.smoothedPressure - sensorState.previousSmoothedPressure) / sensorState.timeSinceLastRead_pres_ms;
    
  }
}

void SystemFSM::updateFlow(void) {
    // Calculate pump flow and update the state
    sensorState.pumpFlow = hw.getPumpFlow(sensorState.timeSinceLastRead_pres_s, sensorState.pumpClicks, sensorState.smoothedPressure);

    // Save the previous smoothed pump flow
    sensorState.previousSmoothedPumpFlow = sensorState.smoothedPumpFlow;

    // Apply smoothing to the pump flow
    sensorState.smoothedPumpFlow = smoothPumpFlow.updateEstimate(sensorState.pumpFlow);

    // Calculate the rate of change of the pump flow
    sensorState.pumpFlowChangeSpeed = (sensorState.smoothedPumpFlow - sensorState.previousSmoothedPumpFlow) / sensorState.timeSinceLastRead_pres_s;
}

inline void SystemFSM::handleBrewing() {
  static int brewFSMCount = 0;
  static BrewState brewingState = BrewState::FillBoiler;

  BLOG_DEBUG("Handling Brewing FSM...\n");
  if(SystemState::Brewing != handleSystemIdleing())
  {
    brewingState=BrewState::FillBoiler;
    return;
  }
  brewFSMCount++;
  switch (brewingState)
  {

  case BrewState::FillBoiler:
    BLOG_ERROR("State: FillBoiler - Filling the boiler...\n");
    hw.fillBoiler();
    sensorState.setTimeSinceBrewStart();
    brewingState = BrewState::WaitingForIdle;
      
   
    break;
  case BrewState::WaitingForIdle:
  if (hw.isBoilerFull(sensorState)) {
        BLOG_DEBUG("Boiler filled. Transitioning to Idle state.\n");
        hw.stopFillBoiler();
        brewingState = BrewState::Idle;
    } else {
        BLOG_DEBUG("Waiting for boiler to be filled.\n");
    }
    break;
  case BrewState::Brew:
    BLOG_DEBUG("State: Brew - Brewing in progress...\n");
    if (brewFSMCount > TOTAL_ITERATIONS)
    {
      BLOG_DEBUG("Brewing complete. Transitioning to Complete state.\n");
      brewingState = BrewState::Complete;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate idle delay
      BLOG_DEBUG("Brewing complete. Transitioning to Complete state.\n");
    brewingState = BrewState::Brew;
    break;
  case BrewState::Complete:
    BLOG_DEBUG("State: Complete - Brewing process is complete.\n");
    currentState = SystemState::Complete; // Transition to System FSM's Complete state
    brewFSMCount = 0;                     // Reset counter    
    break;

  default:
    BLOG_ERROR("Unknown brewing state!\n");
    assert( 0 && "unknown state");
    brewingState = BrewState::FillBoiler; // Reset to safe state
    break;
  }
}


void SystemFSM::handleComplete() {
  BLOG_DEBUG("Brewing process complete. Enjoy your beverage!\n");
  BLOG_ERROR("Type 'exit' to stop or 'restart' to initialize again: \n");
  std::string input;
  std::cin >> input;
  if (input == "exit")
  {
    currentState = SystemState::Exit;
  }
  else if (input == "restart")
  {
    currentState = SystemState::Initialize;
  }
  else
  {
    BLOG_ERROR("Invalid input. Staying in Complete state.\n");
  }
}


inline bool SystemFSM::step() {
  switch (currentState)
  {
  case SystemState::Initialize:
    handleInitialize();
    break;
  case SystemState::SystemIdeling:
    handleSystemIdleing();
    break;
  case SystemState::Brewing:
    handleBrewing();
    break;
  case SystemState::Complete:
    handleComplete();
    break;
  case SystemState::Exit:
    BLOG_ERROR("Exiting system...\n");
    return false;
  default:
    BLOG_ERROR("Unknown state! Exiting for safety.\n");
    currentState = SystemState::Exit;
    break;
  }
  return true;
}

void SystemFSM::run() {
  Iteration currentIteration = {};
  Iteration previousIteration = {};
  int currIndex = 0;

  while (currentState != SystemState::Exit)
  {
    //std::memset(&currentIteration, 0, sizeof(Iteration));
    currentIteration={};
    currentIteration.index = currIndex;
    currentIteration.start = std::chrono::high_resolution_clock::now();

    // Log previous iteration timings
    BLOG_ERROR(
      "Total Time: %f ms Execution Time: %f ms Sleep Time: %f ms\n",
      previousIteration.totalTime_ms,
      std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(previousIteration.elapsed).count(),
      previousIteration.sleeptime_ns / 1e6 // Convert nanoseconds to milliseconds
    );

    if (!step())
    {
      break;
    }

    // Calculate total time for the current iteration
    currentIteration.totalTime_ms = calculateTotalTime(currentIteration.start, previousIteration.start);

    // Mark the end of the current iteration
    currentIteration.end = std::chrono::high_resolution_clock::now();
    currentIteration.elapsed = currentIteration.end - currentIteration.start;

    // Execute sleep and update sleep time in the struct
    currentIteration.sleeptime_ns = executeSleep(currentIteration.elapsed);
    // if (currentIteration.sleeptime_ns == 0.0) {
    //     currentIteration.exceeds_ten = true;
    // }

    // Update the previous iteration
    previousIteration = currentIteration;
    currIndex++;
  }
}

// inline double SystemFSM::calculateTotalTime(
//     const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
//     const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart
// ) {
//     auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart);
//     return duration.count() / 1e6; // Convert nanoseconds to milliseconds
// }

inline double SystemFSM::calculateTotalTime(
  const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
  const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart
) {
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currStart - prevStart);
  return duration.count(); // Already in milliseconds
}


inline double SystemFSM::executeSleep(const std::chrono::duration<double, std::nano>& elapsed) {
  constexpr long maxNsSleep = 10'000'00000; // 10ms in nanoseconds
  constexpr long minNsSleep = 1'000'000;  // 1ms in nanoseconds

  // Calculate remaining sleep time
  double sleepTime = maxNsSleep - elapsed.count();

  if (sleepTime > 0 && sleepTime >= minNsSleep)
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleepTime)));
    return sleepTime; // Return the calculated sleep time
  }
  else if (sleepTime > 0)
  {
    // Sleep time is too small, log the decision to skip sleep
    //BLOG_DEBUG("Skipping sleep as the calculated time is less than 1 ms\n");
  }
  else
  {
    // Elapsed time exceeded the maximum allowed sleep time
    BLOG_ERROR("Exceeded maximum sleep time. Skipping sleep.\n");
  }

  return 0.0; // Indicate that no sleep was performed
}