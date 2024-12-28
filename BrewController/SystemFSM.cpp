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
    if (logModulator % LOG_ITERATION_COUNT_10S == 0)
    {
      BLOG_ERROR("%s\n", sensorState.toString().c_str());
    }

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
    hw.SetBoiler(true); // Turn on the boiler
    sensorState.setTimeSinceBrewStart();
    brewingState = BrewState::WaitingForBoilerFull;


    break;
  case BrewState::WaitingForBoilerFull:
    if (hw.isBoilerFull(sensorState))
    {
      BLOG_ERROR("Boiler filled. Transitioning to Brew state.\n");
      hw.stopFillBoiler();

      brewingState = BrewState::RampToStartTemp;
    }
    else
    {
      if (brewFSMCount % LOG_ITERATION_COUNT_10S == 0)
      {
        BLOG_DEBUG("Waiting for boiler to be filled.\n");
      }
    }
    break;
  case BrewState::RampToStartTemp:
    BLOG_DEBUG("State: RampToStartTemp - Heating boiler to start condition...\n");

    // Check if the current temperature meets or exceeds the start condition
    if (sensorState.temperature >= selectedProfile.StartTemp)
    {
      BLOG_ERROR("Boiler reached start condition temperature %f. Transitioning to Brew state.\n", sensorState.temperature);
      hw.SetBoiler(false); // Stop heating
      sensorState.setTimeSinceStartReached();
      brewingState = BrewState::Brew;
    }
    if (brewFSMCount % LOG_ITERATION_COUNT_10S == 0)
    {
      BLOG_ERROR("Current temperature: %.2f, Target: %.2f\n", sensorState.temperature, selectedProfile.StartTemp);
    }
    break;
  case BrewState::Brew:
    if (brewFSMCount % LOG_ITERATION_COUNT_10S == 0)
    {
      BLOG_DEBUG("State: Brew - Brewing in progress...\n");
    }

    brewingState = DoBrew();
    hw.SetBoiler(sensorState.brewCtrl_heater_state);
    hw.SetPump(sensorState.brewCtrl_pressure_output);
    if (sensorState.StartBrewingwithValveOpen)
    {
      hw.openValve();
    }
    break;
  case BrewState::Complete:
    BLOG_ERROR("Brew Complete\n");
    hw.Reset();
    currentState = SystemState::BrewingComplete; // Transition to System FSM's Complete state
    brewFSMCount = 0;                     // Reset counter    
    break;
  }
}


void SystemFSM::handleBrewingComplete() {
  static int logModulator = 0;
  if (false == sensorState.brewSwitchState)
  {
    {
      if (logModulator % LOG_ITERATION_COUNT_10S == 0)
      {
        BLOG_ERROR("BREW switched off...\n");
      }
    }
    currentState = SystemState::SystemIdeling;
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




std::map<std::string, BrewProfile> SystemFSM::loadBrewProfiles(const std::string& filename) {
  std::map<std::string, BrewProfile> profiles;
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

    BrewProfile brewProfile;
    brewProfile.name = profile["name"].asString();
    brewProfile.StartTemp = profile.get("StartTemp", 95.0).asDouble();

    for (const auto& point : profile["points"])
    {
      if (!point.isMember("time") || !point.isMember("duration") ||
        !point.isMember("pressure") || !point.isMember("temperature"))
      {
        BLOG_ERROR("Invalid point format in profile '%s'. Skipping point.\n", brewProfile.name.c_str());
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
        BLOG_ERROR("Missing pressure PID in profile '%s'.\n", brewProfile.name.c_str());
      }

      if (point.isMember("temperaturePID"))
      {
        bp.temperaturePID.Kp = point["temperaturePID"].get("Kp", 0.0).asDouble();
        bp.temperaturePID.Ki = point["temperaturePID"].get("Ki", 0.0).asDouble();
        bp.temperaturePID.Kd = point["temperaturePID"].get("Kd", 0.0).asDouble();
      }
      else
      {
        BLOG_ERROR("Missing temperature PID in profile '%s'.\n", brewProfile.name.c_str());
      }

      brewProfile.points.push_back(bp);
    }

    profiles[brewProfile.name] = brewProfile;
    BLOG_INFO("Loaded profile: %s\n", brewProfile.name.c_str());
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


  if ( sensorState.brewCtrl_currentBrewProfileIndex < profile.size() - 1)
  {
    const auto& current = profile[ sensorState.brewCtrl_currentBrewProfileIndex];
    const auto& next = profile[ sensorState.brewCtrl_currentBrewProfileIndex + 1];
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
    ++ sensorState.brewCtrl_currentBrewProfileIndex;
  }

  return profile.back();
}

// // // Logging function
// void SystemFSM::logData(std::ofstream& logFile, double elapsed_time, double actual_pressure, double actual_temperature, double pressure_output, double heater_state, double flow_rate) {
//   std::stringstream s;
//   s << std::fixed << std::setprecision(2) << elapsed_time << ", "
//     << actual_pressure << ", " << actual_temperature << ", "
//     << pressure_output << ", " << heater_state << ", " << flow_rate << "\n";
//   logFile << s.str();
//   std::cout << s.str() << std::endl;
// }

void SystemFSM::logData(std::ofstream& logFile, double elapsed_time, double actual_pressure, double actual_temperature, 
                        double pressure_output, double heater_state, double flow_rate, 
                        double error_temp, double error_pressure, double pwm_duty_cycle) {
    std::stringstream s;
    s << std::fixed << std::setprecision(2)
      << "Time: " << elapsed_time << " s, "
      << "Pressure: " << actual_pressure << " bar, "
      << "Temperature: " << actual_temperature << " °C, "
      << "Pressure Output: " << pressure_output << " %, "
      << "Heater State: " << (heater_state ? "ON" : "OFF") << ", "
      << "Flow Rate: " << flow_rate << " mL/s, "
      << "Temp Error: " << error_temp << ", "
      << "Pressure Error: " << error_pressure << ", "
      << "PWM Duty Cycle: " << pwm_duty_cycle * 100.0 << " %\n";
    logFile << s.str();
    std::cout << s.str();
}

// BrewState SystemFSM::DoBrew() {
//   // Calculate elapsed time
//   auto now = std::chrono::steady_clock::now();
//   double elapsed_time_sec = sensorState.getTimeSinceStartReached() / 1000.0;
//   double pwm_elapsed_time_sec = elapsed_time_sec;

//   // Get the current target point
//   BrewPoint target = getTargetPoint(selectedProfile.points, elapsed_time_sec);

//   // Temperature control (PID with flow rate compensation)
//   double error_temp = target.temperature - sensorState.temperature;
//   sensorState.integral_temp += error_temp * CONTROL_LOOP_INTERVAL;
//   double derivative_temp = (error_temp - sensorState.prev_error_temp) / CONTROL_LOOP_INTERVAL;
//   double temp_output = target.temperaturePID.Kp * error_temp
//     + target.temperaturePID.Ki * sensorState.integral_temp
//     + target.temperaturePID.Kd * derivative_temp
//     + TEMP_FEEDFORWARD_GAIN * sensorState.consideredFlow;

//   // Clamp the output to allowable duty cycle range
//   double pwm_duty_cycle = std::clamp(temp_output / 100.0, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

//   // PWM logic for heater control
//   if (pwm_elapsed_time_sec >= MAX_DUTY_CYCLE)
//   {
//     sensorState.heater_pwm_start_time = now; // Reset PWM cycle
//     pwm_elapsed_time_sec = 0.0;
//   }
//   sensorState.heater_state = (pwm_elapsed_time_sec < pwm_duty_cycle);

//   // Pressure control (PID with flow rate compensation)
//   double error_pressure = target.pressure - sensorState.smoothedPressure;
//   sensorState.integral_pressure += error_pressure * CONTROL_LOOP_INTERVAL;
//   double derivative_pressure = (error_pressure - sensorState.prev_error_pressure) / CONTROL_LOOP_INTERVAL;
//   sensorState.pressure_output = target.pressurePID.Kp * error_pressure
//     + target.pressurePID.Ki * sensorState.integral_pressure
//     + target.pressurePID.Kd * derivative_pressure
//     + PRESSURE_FEEDFORWARD_GAIN * sensorState.consideredFlow;

//   // Log data
//   logData(logFile, elapsed_time_sec, sensorState.smoothedPressure, sensorState.temperature,
//     sensorState.pressure_output, sensorState.heater_state, sensorState.consideredFlow);

//   // Update previous errors
//   sensorState.prev_error_temp = error_temp;
//   sensorState.prev_error_pressure = error_pressure;

//   // Open brew valve if target pressure is reached
//   if (sensorState.smoothedPressure >= target.pressure)
//   {
//     sensorState.StartBrewingwithValveOpen = true;
//   }
//   // Exit condition: stop after the last point's time and duration
//   double total_duration = selectedProfile.points.back().time + selectedProfile.points.back().duration;
//   if (elapsed_time_sec > total_duration)
//   {

//     BLOG_ERROR("elapsed time %f, total duration %f\n", elapsed_time_sec, total_duration);
//     sensorState.pressure_output = 0;
//     temp_output = 0;
//     return BrewState::Complete;
//   }

//   return BrewState::Brew;
// }

// BrewState SystemFSM::DoBrew() {
//     // Calculate elapsed time
//     auto now = std::chrono::steady_clock::now();
//     double elapsed_time_sec = sensorState.getTimeSinceStartReached() / 1000.0;
//     double pwm_elapsed_time_sec = elapsed_time_sec;

//     // Get the current target point
//     BrewPoint target = getTargetPoint(selectedProfile.points, elapsed_time_sec);

//     // Temperature control (PID with flow rate compensation)
//     double error_temp = target.temperature - sensorState.temperature;
//     sensorState.integral_temp += error_temp * CONTROL_LOOP_INTERVAL;
//     double derivative_temp = (error_temp - sensorState.prev_error_temp) / CONTROL_LOOP_INTERVAL;
//     double temp_output = target.temperaturePID.Kp * error_temp
//                        + target.temperaturePID.Ki * sensorState.integral_temp
//                        + target.temperaturePID.Kd * derivative_temp
//                        + TEMP_FEEDFORWARD_GAIN * sensorState.consideredFlow;

//     // Clamp the output to allowable duty cycle range
//     double pwm_duty_cycle = std::clamp(temp_output / 100.0, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

//     // PWM logic for heater control
//     if (pwm_elapsed_time_sec >= MAX_DUTY_CYCLE) {
//         sensorState.heater_pwm_start_time = now; // Reset PWM cycle
//         pwm_elapsed_time_sec = 0.0;
//     }
//     sensorState.heater_state = (pwm_elapsed_time_sec < pwm_duty_cycle);

//     // Pressure control (PID with flow rate compensation)
//     double error_pressure = target.pressure - sensorState.smoothedPressure;
//     sensorState.integral_pressure += error_pressure * CONTROL_LOOP_INTERVAL;
//     double derivative_pressure = (error_pressure - sensorState.prev_error_pressure) / CONTROL_LOOP_INTERVAL;
//     double pressure_output_raw = target.pressurePID.Kp * error_pressure
//                                + target.pressurePID.Ki * sensorState.integral_pressure
//                                + target.pressurePID.Kd * derivative_pressure
//                                + PRESSURE_FEEDFORWARD_GAIN * sensorState.consideredFlow;

//     // Clamp pressure output to the range [0, 100]
//     sensorState.pressure_output = std::clamp(pressure_output_raw, 0.0, 100.0);

//     // Log data
//     logData(logFile, elapsed_time_sec, sensorState.smoothedPressure, sensorState.temperature,
//             sensorState.pressure_output, sensorState.heater_state, sensorState.consideredFlow,
//             error_temp, error_pressure, pwm_duty_cycle);

//     // Update previous errors
//     sensorState.prev_error_temp = error_temp;
//     sensorState.prev_error_pressure = error_pressure;

//     // Open brew valve if target pressure is reached
//     if (sensorState.smoothedPressure >= target.pressure) {
//         sensorState.StartBrewingwithValveOpen = true;
     
//     }

//     // Exit condition: stop after the last point's time and duration
//     double total_duration = selectedProfile.points.back().time + selectedProfile.points.back().duration;
//     if (elapsed_time_sec > total_duration) {
//         BLOG_ERROR("Elapsed time: %.3f sec, Total duration: %.3f sec\n", elapsed_time_sec, total_duration);
//         sensorState.pressure_output = 0;
//         temp_output = 0;
//         return BrewState::Complete;
//     }

//     return BrewState::Brew;
// }
BrewState SystemFSM::DoBrew() {
    // Calculate elapsed time
    auto now = std::chrono::steady_clock::now();
    sensorState.brewCtrl_elapsed_time_sec = sensorState.getTimeSinceStartReached() / 1000.0; // Store in struct
    sensorState.brewCtrl_pwm_elapsed_time_sec = sensorState.brewCtrl_elapsed_time_sec;

    // Update brew target from the current profile
    sensorState.brewCtrl_target = getTargetPoint(selectedProfile.points, sensorState.brewCtrl_elapsed_time_sec);

    // Temperature control (PID with flow rate compensation)
    sensorState.brewCtrl_error_temp = sensorState.brewCtrl_target.temperature - sensorState.temperature;
    sensorState.integral_temp += sensorState.brewCtrl_error_temp * CONTROL_LOOP_INTERVAL;
    sensorState.brewCtrl_derivative_temp = (sensorState.brewCtrl_error_temp - sensorState.prev_error_temp) / CONTROL_LOOP_INTERVAL;
    sensorState.brewCtrl_temp_output = sensorState.brewCtrl_target.temperaturePID.Kp * sensorState.brewCtrl_error_temp
                                     + sensorState.brewCtrl_target.temperaturePID.Ki * sensorState.integral_temp
                                     + sensorState.brewCtrl_target.temperaturePID.Kd * sensorState.brewCtrl_derivative_temp;

    // Clamp the output to allowable duty cycle range
    sensorState.brewCtrl_pwm_duty_cycle = std::clamp(sensorState.brewCtrl_temp_output / 100.0, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    // PWM logic for heater control
    if (sensorState.brewCtrl_pwm_elapsed_time_sec >= MAX_DUTY_CYCLE) {
        sensorState.heater_pwm_start_time = now; // Reset PWM cycle
        sensorState.brewCtrl_pwm_elapsed_time_sec = 0.0;
    }
    sensorState.brewCtrl_heater_state = (sensorState.brewCtrl_pwm_elapsed_time_sec < sensorState.brewCtrl_pwm_duty_cycle);

    // Pressure control (PID with flow rate compensation)
    sensorState.brewCtrl_error_pressure = sensorState.brewCtrl_target.pressure - sensorState.smoothedPressure;
    sensorState.integral_pressure += sensorState.brewCtrl_error_pressure * CONTROL_LOOP_INTERVAL;
    sensorState.brewCtrl_derivative_pressure = (sensorState.brewCtrl_error_pressure - sensorState.prev_error_pressure) / CONTROL_LOOP_INTERVAL;
    sensorState.brewCtrl_pressure_output = sensorState.brewCtrl_target.pressurePID.Kp * sensorState.brewCtrl_error_pressure
                                         + sensorState.brewCtrl_target.pressurePID.Ki * sensorState.integral_pressure
                                         + sensorState.brewCtrl_target.pressurePID.Kd * sensorState.brewCtrl_derivative_pressure;

    // Clamp pressure output
    sensorState.brewCtrl_pressure_output = std::clamp(sensorState.brewCtrl_pressure_output, 0.0, 100.0);

    // Open brew valve if target pressure is reached
    if (sensorState.smoothedPressure >= sensorState.brewCtrl_target.pressure) {
        sensorState.StartBrewingwithValveOpen = true;
    }
   

    // Log data
    logData(logFile, sensorState.brewCtrl_elapsed_time_sec, sensorState.smoothedPressure, sensorState.temperature,
            sensorState.brewCtrl_pressure_output, sensorState.brewCtrl_heater_state, sensorState.consideredFlow, sensorState.brewCtrl_error_temp,sensorState.brewCtrl_error_pressure, sensorState.brewCtrl_pwm_duty_cycle);

    // Update previous errors
    sensorState.prev_error_temp = sensorState.brewCtrl_error_temp;
    sensorState.prev_error_pressure = sensorState.brewCtrl_error_pressure;

    // Exit condition: stop after the last point's time and duration
    double total_duration = selectedProfile.points.back().time + selectedProfile.points.back().duration;
    if (sensorState.brewCtrl_elapsed_time_sec > total_duration) {
        BLOG_ERROR("Elapsed time: %.3f sec, Total duration: %.3f sec\n", sensorState.brewCtrl_elapsed_time_sec, total_duration);
        sensorState.brewCtrl_pressure_output = 0;
        sensorState.brewCtrl_temp_output = 0;
        return BrewState::Complete;
    }

    return BrewState::Brew;
}


void SystemFSM::WriteLogHeader() {
//  logFile << "Time(s), Pressure(bar), Temperature(C), Pump Output, Heater State, Flow Rate(mL/s)\n";
}
