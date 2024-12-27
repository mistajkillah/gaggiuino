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
  BrewSetup,
  Brewing,
  Complete,
  Exit
};

enum class BrewState { FillBoiler, WaitingForIdle, Idle, Brew, Complete };
constexpr int TOTAL_ITERATIONS = 10;

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

class SystemFSM
{
private:
  SystemState currentState=SystemState::Initialize;
  BrewHW& hw;
  SensorState sensorState = {};
  // Private helper functions for each state
  void handleInitialize();
  SystemState handleSystemIdleing();
  bool isBoilerFull();
  void handleRpc();
  void handleBrewSetup();
  void fillBoiler();
  void updatePressue(void);
  void updateFlow(void);
inline void handleBrewing();
  void handleComplete();

  void calcMath();

  inline double executeSleep(const std::chrono::duration<double, std::nano>& elapsed);
  inline double calculateTotalTime(
    const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
    const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart);
  SimpleKalmanFilter smoothPressure{0.6f, 0.6f, 0.1f};
  SimpleKalmanFilter smoothPumpFlow{0.1f, 0.1f, 0.01f};
  SimpleKalmanFilter smoothScalesFlow{0.5f, 0.5f, 0.01f};
  SimpleKalmanFilter smoothConsideredFlow{0.1f, 0.1f, 0.1f};
public:
  SystemFSM();
  inline bool step();
  void run();
};
