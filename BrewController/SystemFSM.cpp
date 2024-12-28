#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <map>
#include <future>
#include <json/json.h>
#include <sqlite3.h>

#include "BrewController.h"
#include "SensorStructs.h"
#include "SimpleKalmanFilter.h"
#include "measurements.h"
#include "SystemFSM.h"
#include "BrewHW.h"
#include "BrewDB.h"



// Constructor
SystemFSM::SystemFSM() : hw(BrewHW::getInstance()), logFile("control_log.csv") {}

void SystemFSM::handleInitialize() {
  BLOG_INFO("System is initializing...\n");
  int ret = system("rm -rf /tmp/mydb.sqlite");
  BLOG_INFO("removing old db %d\n", ret);
  BrewDB& brewDB = BrewDB::getInstance();
  brewDB.Initialize();
  hw.initializeHW();
  brewProfiles = loadBrewProfiles(brewProfilesFileName);
  sensorState.setTimeSinceSystemStart();

  // Validate the selected profile
  if (brewProfiles.find(selectBrewProfile) == brewProfiles.end())
  {
    BLOG_ERROR("Cannot find %s profile\n", selectBrewProfile.c_str());
    currentState = SystemState::Exit;
  }
  else
  {
    selectedProfile = brewProfiles[selectBrewProfile];
    currentState = SystemState::SystemIdeling;
  }

}

void SystemFSM::updateSensorStateAsync() {
  static int i = 0;
  std::future<float> pressureFuture;
  std::future<float> temperatureFuture;

  // Check and start async tasks based on conditions
  if (sensorState.getTimeSincelastRead_pres() > GET_PRESSURE_READ_EVERY)
  {
    // Use a lambda to capture 'hw' explicitly
    pressureFuture = std::async(std::launch::async, [this]() {
      return hw.getPressure();
      });
  }
  if (sensorState.getTimeSincelastRead_temp() > GET_KTYPE_READ_EVERY)
  {
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
  if (sensorState.getTimeSincelastRead_temp() > GET_KTYPE_READ_EVERY)
  {
    sensorState.updateTemperatureReadTime();
    if (temperatureFuture.valid())
    {
      sensorState.temperature = temperatureFuture.get();
    }
  }

  // Wait for pressure and update state
  if (sensorState.getTimeSincelastRead_pres() > GET_PRESSURE_READ_EVERY)
  {
    sensorState.updatePressureReadTime();
    if (pressureFuture.valid())
    {
      sensorState.pressure = pressureFuture.get();
    }
    updatePressue(); // Additional processing for pressure
  }

  i++;
}

SystemState SystemFSM::handleSystemIdleing() {
  static int logModulator = 0;
  updateSensorStateAsync();
  //hw.updateSensorStateAsync(sensorState);
  //updatePressue();
  // BLOG_ERROR("%s\n", sensorState.toString().c_str());
  if (false == sensorState.brewSwitchState)
  {
    if (logModulator % LOG_ITERATION_COUNT_10S == 0)
    {
      BLOG_ERROR("BREW switched off...\n");
    }
    currentState = SystemState::SystemIdeling;

  }
  else
  {
    currentState = SystemState::Brewing;
    BLOG_ERROR("%s\n", sensorState.toString().c_str());
  }
  logModulator++;
  return currentState;
}



void SystemFSM::updatePressue(void) {
  //uint32_t elapsedTime = millis() - pressureTimer;
  if (sensorState.timeSinceLastRead_pres_ms > GET_PRESSURE_READ_EVERY)
  {


    sensorState.previousSmoothedPressure = sensorState.smoothedPressure;
    sensorState.smoothedPressure = smoothPressure.updateEstimate(sensorState.pressure);
    sensorState.pressureChangeSpeed = (sensorState.smoothedPressure - sensorState.previousSmoothedPressure) / sensorState.timeSinceLastRead_pres_s;

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
  if (SystemState::Brewing != handleSystemIdleing())
  {
    brewingState = BrewState::FillBoiler;
    return;
  }
  brewFSMCount++;
  switch (brewingState)
  {

  case BrewState::FillBoiler:
    BLOG_ERROR("State: FillBoiler - Filling the boiler...\n");
    hw.fillBoiler();
    sensorState.setTimeSinceBrewStart();
    brewingState = BrewState::WaitingForBoilerFull;


    break;
  case BrewState::WaitingForBoilerFull:
    if (hw.isBoilerFull(sensorState))
    {
      BLOG_ERROR("Boiler filled. Transitioning to Brew state.\n");
      hw.stopFillBoiler();
      sensorState.setTimeSinceBoilerFull();
      brewingState = BrewState::Brew;
    }
    else
    {
      BLOG_DEBUG("Waiting for boiler to be filled.\n");
    }
    break;
  case BrewState::Brew:
    BLOG_DEBUG("State: Brew - Brewing in progress...\n");
    brewingState = DoBrew();
    hw.SetBoiler(sensorState.heater_state);
    hw.SetPump(sensorState.pressure_output);
    break;
  case BrewState::Complete:
    BLOG_ERROR("State: Complete - Brewing process is complete.\n");
    hw.Reset();
    currentState = SystemState::SystemIdeling; // Transition to System FSM's Complete state
    brewFSMCount = 0;                     // Reset counter    
    break;
  }
}


void SystemFSM::handleBrewingComplete() {
  static int logModulator = 0;
  BLOG_ERROR("Brewing process complete. Enjoy your beverage!\n");
  if (false == sensorState.brewSwitchState)
  {
    {
      if (logModulator % LOG_ITERATION_COUNT_10S == 0)
      {
        BLOG_ERROR("BREW switched off...\n");
      }
    }
    currentState = SystemState::BrewingComplete;
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
  case SystemState::BrewingComplete:
    handleBrewingComplete();
    break;
  case SystemState::Exit:
    BLOG_ERROR("Exiting system...\n");
    if (logFile.is_open())
    {
      logFile.close();
    }
    return false;
    // default:
    //   BLOG_ERROR("Unknown state! Exiting for safety.\n");
    //   currentState = SystemState::Exit;
    //   break;
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
    currentIteration = {};
    currentIteration.index = currIndex;
    currentIteration.start = std::chrono::high_resolution_clock::now();

    // // Log previous iteration timings
    // BLOG_ERROR(
    //   "Total Time: %f ms Execution Time: %f ms Sleep Time: %f ms\n",
    //   previousIteration.totalTime_ms,
    //   std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(previousIteration.elapsed).count(),
    //   previousIteration.sleeptime_ns / 1e6 // Convert nanoseconds to milliseconds
    // );


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
    if (currentIteration.sleeptime_ns <= 0)
    {
      BLOG_ERROR(
        "Execution Time Exceeded: Total Time: %f ms Execution Time: %f ms Sleep Time: %f ms\n",
        previousIteration.totalTime_ms,
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(previousIteration.elapsed).count(),
        previousIteration.sleeptime_ns / 1e6 // Convert nanoseconds to milliseconds
      );
    }
    // if (currentIteration.sleeptime_ns == 0.0) {
    //     currentIteration.exceeds_ten = true;
    // }

    // Update the previous iteration
    previousIteration = currentIteration;
    currIndex++;
  }
}


inline double SystemFSM::calculateTotalTime(
  const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
  const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart
) {
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currStart - prevStart);
  return duration.count(); // Already in milliseconds
}


inline double SystemFSM::executeSleep(const std::chrono::duration<double, std::nano>& elapsed) {

  // Calculate remaining sleep time
  double sleepTime = MAX_NS_SLEEP - elapsed.count();

  if (sleepTime >= MIN_NS_SLEEP)
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleepTime)));
    return sleepTime; // Return the calculated sleep time
  }
  else if (sleepTime > 0)
  {
    return sleepTime;
    // Sleep time is too small, log the decision to skip sleep
    //BLOG_DEBUG("Skipping sleep as the calculated time is less than 1 ms\n");
  }
  else
  {
    // // Elapsed time exceeded the maximum allowed sleep time
    // BLOG_ERROR("Exceeded maximum sleep time. Skipping sleep.\n");
        // Log previous iteration timings

    return sleepTime;
  }

  return 0.0; // Indicate that no sleep was performed
}



// // Logging function
void SystemFSM::logData(std::ofstream& logFile, double elapsed_time, double actual_pressure, double actual_temperature, double pressure_output, double heater_state, double flow_rate) {
  std::stringstream s;
  s << std::fixed << std::setprecision(2) << elapsed_time << ", "
    << actual_pressure << ", " << actual_temperature << ", "
    << pressure_output << ", " << heater_state << ", " << flow_rate << "\n";
  logFile << s.str();
  std::cout << s.str() << std::endl;
}

std::map<std::string, std::vector<BrewPoint>> SystemFSM::loadBrewProfiles(const std::string& filename) {
  std::map<std::string, std::vector<BrewPoint>> profiles;
  Json::Value root;
  std::ifstream file(filename, std::ifstream::binary);

  // Check if file exists
  if (!file.is_open())
  {
    BLOG_ERROR("Failed to open brew profiles file: %s\n", filename.c_str());
    return profiles;
  }

  try
  {
    file >> root; // Parse JSON
  }
  catch (const Json::RuntimeError& e)
  {
    BLOG_ERROR("JSON parsing error: %s\n", e.what());
    return profiles;
  }

  // Check if the "BrewProfile" key exists
  if (!root.isMember("BrewProfile") || !root["BrewProfile"].isArray())
  {
    BLOG_ERROR("Invalid JSON format: 'BrewProfile' key is missing or not an array.\n");
    return profiles;
  }

  // Parse the profiles
  for (const auto& profile : root["BrewProfile"])
  {
    if (!profile.isMember("name") || !profile.isMember("points"))
    {
      BLOG_ERROR("Invalid profile format: 'name' or 'points' key is missing.\n");
      continue;
    }

    std::string name = profile["name"].asString();
    std::vector<BrewPoint> points;

    for (const auto& point : profile["points"])
    {
      if (!point.isMember("time") || !point.isMember("duration") ||
        !point.isMember("pressure") || !point.isMember("temperature"))
      {
        BLOG_ERROR("Invalid point format in profile '%s'. Skipping point.\n", name.c_str());
        continue;
      }

      BrewPoint bp;
      bp.time = point["time"].asDouble();
      bp.duration = point["duration"].asDouble();
      bp.pressure = point["pressure"].asDouble();
      bp.temperature = point["temperature"].asDouble();

      // Parse PID parameters
      if (point.isMember("pressurePID"))
      {
        bp.pressurePID.Kp = point["pressurePID"].get("Kp", 0.0).asDouble();
        bp.pressurePID.Ki = point["pressurePID"].get("Ki", 0.0).asDouble();
        bp.pressurePID.Kd = point["pressurePID"].get("Kd", 0.0).asDouble();
      }
      else
      {
        BLOG_ERROR("missing pressure pid\n");
      }


      if (point.isMember("temperaturePID"))
      {
        bp.temperaturePID.Kp = point["temperaturePID"].get("Kp", 0.0).asDouble();
        bp.temperaturePID.Ki = point["temperaturePID"].get("Ki", 0.0).asDouble();
        bp.temperaturePID.Kd = point["temperaturePID"].get("Kd", 0.0).asDouble();
      }
      else
      {
        BLOG_ERROR("missing temp pid\n");
      }

      points.push_back(bp);
    }

    profiles[name] = points;
  }

  BLOG_INFO("Successfully loaded brew profiles from %s\n", filename.c_str());
  return profiles;
}

// // Read BrewProfiles from JSON file
// std::map<std::string, std::vector<BrewPoint>> SystemFSM::loadBrewProfiles(const std::string& filename) {
//   std::map<std::string, std::vector<BrewPoint>> profiles;
//   Json::Value root;
//   std::ifstream file(filename, std::ifstream::binary);
//   file >> root;

//   for (const auto& profile : root["BrewProfile"])
//   {
//     std::string name = profile["name"].asString();
//     std::vector<BrewPoint> points;

//     for (const auto& point : profile["points"])
//     {
//       BrewPoint bp;
//       bp.time = point["time"].asDouble();
//       bp.duration = point["duration"].asDouble();
//       bp.pressure = point["pressure"].asDouble();
//       bp.temperature = point["temperature"].asDouble();

//       // Parse PID parameters
//       bp.pressurePID.Kp = point["pressurePID"]["Kp"].asDouble();
//       bp.pressurePID.Ki = point["pressurePID"]["Ki"].asDouble();
//       bp.pressurePID.Kd = point["pressurePID"]["Kd"].asDouble();
//       bp.temperaturePID.Kp = point["temperaturePID"]["Kp"].asDouble();
//       bp.temperaturePID.Ki = point["temperaturePID"]["Ki"].asDouble();
//       bp.temperaturePID.Kd = point["temperaturePID"]["Kd"].asDouble();

//       points.push_back(bp);
//     }

//     profiles[name] = points;
//   }
//   return profiles;
// }
BrewPoint SystemFSM::getTargetPoint(const std::vector<BrewPoint>& profile, double elapsed_time) {
  unsigned int currentBrewProfileIndex = sensorState.currentBrewProfileIndex;

  if (currentBrewProfileIndex < profile.size() - 1)
  {
    const auto& current = profile[currentBrewProfileIndex];
    const auto& next = profile[currentBrewProfileIndex + 1];
    double end_time = current.time + current.duration;

    if (elapsed_time < end_time)
    {
      return current;
    }
    else if (elapsed_time < next.time)
    {
      double ramp_progress = (elapsed_time - end_time) / (next.time - end_time);
      return {
          elapsed_time,
          0.0,
          current.pressure + ramp_progress * (next.pressure - current.pressure),
          current.temperature + ramp_progress * (next.temperature - current.temperature),
          current.pressurePID,
          current.temperaturePID
      };
    }

    // Move to the next index
    ++currentBrewProfileIndex;
  }

  return profile.back();
}


BrewState SystemFSM::DoBrew() {
  // Calculate elapsed time
  auto now = std::chrono::steady_clock::now();
  double elapsed_time = sensorState.getTimeSinceBoilerFull();
  double pwm_elapsed_time = elapsed_time;

  // Get the current target point
  BrewPoint target = getTargetPoint(selectedProfile, elapsed_time);

  // Temperature control (PID with flow rate compensation)
  double error_temp = target.temperature - sensorState.temperature;
  sensorState.integral_temp += error_temp * CONTROL_LOOP_INTERVAL;
  double derivative_temp = (error_temp - sensorState.prev_error_temp) / CONTROL_LOOP_INTERVAL;
  double temp_output = target.temperaturePID.Kp * error_temp
    + target.temperaturePID.Ki * sensorState.integral_temp
    + target.temperaturePID.Kd * derivative_temp
    + TEMP_FEEDFORWARD_GAIN * sensorState.consideredFlow;

  // Clamp the output to allowable duty cycle range
  double pwm_duty_cycle = std::clamp(temp_output / 100.0, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

  // PWM logic for heater control
  if (pwm_elapsed_time >= MAX_DUTY_CYCLE)
  {
    sensorState.heater_pwm_start_time = now; // Reset PWM cycle
    pwm_elapsed_time = 0.0;
  }
  sensorState.heater_state = (pwm_elapsed_time < pwm_duty_cycle);

  // Pressure control (PID with flow rate compensation)
  double error_pressure = target.pressure - sensorState.smoothedPressure;
  sensorState.integral_pressure += error_pressure * CONTROL_LOOP_INTERVAL;
  double derivative_pressure = (error_pressure - sensorState.prev_error_pressure) / CONTROL_LOOP_INTERVAL;
  sensorState.pressure_output = target.pressurePID.Kp * error_pressure
    + target.pressurePID.Ki * sensorState.integral_pressure
    + target.pressurePID.Kd * derivative_pressure
    + PRESSURE_FEEDFORWARD_GAIN * sensorState.consideredFlow;

  // Log data
  logData(logFile, elapsed_time, sensorState.smoothedPressure, sensorState.temperature,
    sensorState.pressure_output, sensorState.heater_state, sensorState.consideredFlow);

  // Update previous errors
  sensorState.prev_error_temp = error_temp;
  sensorState.prev_error_pressure = error_pressure;

  // Exit condition: stop after the last point's time and duration
  double total_duration = selectedProfile.back().time + selectedProfile.back().duration;
  if (elapsed_time > total_duration)
  {
    sensorState.pressure_output = 0;
    temp_output = 0;
    return BrewState::Complete;
  }

  return BrewState::Brew;
}


void SystemFSM::WriteLogHeader() {
  logFile << "Time(s), Pressure(bar), Temperature(C), Pump Output, Heater State, Flow Rate(mL/s)\n";
}
