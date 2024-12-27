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
//#include "SimpleKalmanFilter.h"
//#include "measurements.h"
#include "BrewHW.h"
#include "BrewDB.h"

// Enumeration for the different states of the system
enum class SystemState {
  Initialize,
  BrewSetup,
  Brewing,
  Complete,
  Exit
};
constexpr int TOTAL_ITERATIONS = 10;
constexpr double NANO_TO_MSEC = 1e6;

// // Struct for iteration data
// struct Iteration {
//     int index;
//     double sleeptime_ns;
//     bool exceeds_ten;
//     std::chrono::time_point<std::chrono::high_resolution_clock> start;
//     std::chrono::time_point<std::chrono::high_resolution_clock> end;
//     std::chrono::duration<double, std::nano> elapsed;
//     double totalTime_ms;
// };


class SystemFSM {
private:
  SystemState currentState;
  BrewHW& hw;

  // Private helper functions for each state
  void handleInitialize();
  void handleRpc();
  void handleBrewSetup();
  inline void handleBrewing();
  void handleComplete();

  // Helper for latency calculation
  inline double calcualteTotalTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
    const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart);
  inline void executeSleep(Iteration& iter);
  inline void executeSleep(const std::chrono::nanoseconds& elapsedTime);
public:
  SystemFSM();
  inline bool step();
  void run();
};
