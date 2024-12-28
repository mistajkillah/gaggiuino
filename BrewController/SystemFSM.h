#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <sqlite3.h>
#include <mutex>
#include "BrewController.h"
#include "SensorStructs.h"
#include "SimpleKalmanFilter.h"
//#include "measurements.h"
#include "BrewHW.h"
#include "BrewDB.h"

// Enumeration for the different states of the system
enum class SystemState
{
  Initialize,
  SystemIdeling,
  Brewing,
  BrewingComplete,
  Exit
};

enum class BrewState { FillBoiler, WaitingForBoilerFull, Brew, Complete };


struct Iteration
{
  int index;
  double sleeptime_ns = 0.0;
  //bool exceeds_ten = false;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> end;
  std::chrono::duration<double, std::nano> elapsed;
  double totalTime_ms = 0.0;
};

// PID structure to store dynamic parameters
struct PID
{
  double Kp, Ki, Kd;
};

// BrewPoint structure with ramping and dynamic PID parameters
struct BrewPoint
{
  double time;                 // The specific time to start this state (seconds)
  double duration;             // Duration to hold this state (seconds)
  double pressure;             // Desired pressure
  double temperature;          // Desired temperature
  PID pressurePID;             // PID parameters for pressure
  PID temperaturePID;          // PID parameters for temperature
};


class SystemFSM
{
private:
  SystemState currentState = SystemState::Initialize;
  BrewHW& hw;
  SensorState sensorState = {};
  // Private helper functions for each state
  void handleInitialize();
  void updateSensorStateAsync();
  SystemState handleSystemIdleing();
  bool isBoilerFull();
  void handleRpc();
  void handleBrewSetup();
  void fillBoiler();
  void updatePressue(void);
  void updateFlow(void);
  inline void handleBrewing();
  void handleBrewingComplete();

  void calcMath();

  inline double executeSleep(const std::chrono::duration<double, std::nano>& elapsed);
  std::map<std::string, std::vector<BrewPoint>> loadBrewProfiles(const std::string& filename);
  inline double calculateTotalTime(
    const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
    const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart);
  SimpleKalmanFilter smoothPressure{ 0.6f, 0.6f, 0.1f };
  SimpleKalmanFilter smoothPumpFlow{ 0.1f, 0.1f, 0.01f };
  SimpleKalmanFilter smoothScalesFlow{ 0.5f, 0.5f, 0.01f };
  SimpleKalmanFilter smoothConsideredFlow{ 0.1f, 0.1f, 0.1f };
  const std::string brewProfilesFileName = "brew_profiles.json";
  const std::string selectBrewProfile = "Test1";
  std::map<std::string, std::vector<BrewPoint>> brewProfiles;
  std::vector<BrewPoint> selectedProfile;
  std::ofstream logFile;
  BrewPoint getTargetPoint(const std::vector<BrewPoint>& profile, double elapsed_time);
  BrewState DoBrew();
  void WriteLogHeader();
  void logData(std::ofstream& logFile, double elapsed_time, double actual_pressure, double actual_temperature, double pressure_output, double heater_state, double flow_rate);
  // Constants
  // Constants
  static constexpr long MAX_NS_SLEEP = 10000000;      // 10ms in nanoseconds
  static constexpr long MIN_NS_SLEEP = 1000000;       // 1ms in nanoseconds
  static constexpr double CONTROL_LOOP_INTERVAL = MAX_NS_SLEEP / 1000000000.0; // Convert ns to seconds
  static constexpr double MAX_DUTY_CYCLE = 1.0;         // Maximum duty cycle for PWM
  static constexpr double MIN_DUTY_CYCLE = 0.0;         // Minimum duty cycle for PWM
  static constexpr double TEMP_FEEDFORWARD_GAIN = 0.1;  // Feed-forward gain for temperature control
  static constexpr double PRESSURE_FEEDFORWARD_GAIN = 0.1; // Feed-forwar
  static constexpr int LOG_ITERATION_COUNT_10S = static_cast<int>(10 / CONTROL_LOOP_INTERVAL);
public:
  SystemFSM();
  inline bool step();
  void run();
};
