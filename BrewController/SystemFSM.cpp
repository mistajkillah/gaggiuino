
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
#include <deque>
#include "BrewController.h"
#include "SensorStructs.h"
#include "SimpleKalmanFilter.h"
#include "measurements.h"

#include "SystemFSM.h"
#include "BrewHW.h"
#include "BrewDB.h"


// Constructor
SystemFSM::SystemFSM() : hw(BrewHW::getInstance()), currentState(SystemState::Initialize) {}

void SystemFSM::handleInitialize() {
  std::cout << "System is initializing..." << std::endl;
  system("rm -rf /tmp/mydb.sqlite");
  BrewDB& brewDB = BrewDB::getInstance();
  brewDB.Initialize();
  hw.initializeHW();
  currentState = SystemState::BrewSetup;
}

void SystemFSM::handleBrewSetup() {
  std::cout << "Enter brewing information (type 'start' to begin brewing): ";
  std::string input;
  std::cin >> input;
  if (input == "start")
  {
    currentState = SystemState::Brewing;
  }
  else
  {
    std::cout << "Invalid input. Try again." << std::endl;
  }
}

inline void SystemFSM::handleBrewing()
{
    std::cout << "Brewing in progress..." << std::endl;

    SensorState state=hw.getSensorState();
    std::cout<<state.toString()<<std::endl;

    std::cout << "Brewing complete. Transitioning to Complete state." << std::endl;
    currentState = SystemState::Complete;
}
void SystemFSM::run() {
  Iteration currentIteration;
  Iteration previousIteration;

  for (int currIndex = 0; currIndex < TOTAL_ITERATIONS; ++currIndex)
  {
    std::cout << "In Loop index: " << currIndex << std::endl;
    std::memset(&currentIteration, 0, sizeof(Iteration));
    currentIteration.index = currIndex;
    currentIteration.start = std::chrono::high_resolution_clock::now();

    // Print the latency of the previous iteration
    if (currIndex > 0)
    {
      std::cout << "Previous iteration latency: " << previousIteration.latency << " ms" << std::endl;
    }

    
    if(false==step())
    {
      break;
    }


    if (currIndex != 0)
    {
      currentIteration.latency = calculateLatency(currentIteration.start, previousIteration.start);
    }
    else
    {
      currentIteration.latency = 0;
    }

    currentIteration.end = std::chrono::high_resolution_clock::now();
    currentIteration.elapsed = currentIteration.end - currentIteration.start;
    executeSleep(currentIteration);

    // Update previousIteration for the next loop iteration
    previousIteration = currentIteration;
  }
}


void SystemFSM::handleComplete() {
  std::cout << "Brewing process complete. Enjoy your beverage!" << std::endl;
  std::cout << "Type 'exit' to stop or 'restart' to initialize again: ";
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
    std::cout << "Invalid input. Staying in Complete state." << std::endl;
  }
}

void SystemFSM::executeSleep(Iteration& iter) {
  constexpr long maxNsSleep = 10000000; // 10ms
  double sleepTime = 0.0;
  if (iter.elapsed.count() > maxNsSleep)
  {
    iter.exceeds_ten = true;
    std::cerr << "Exceeded maximum sleep time" << std::endl;
  }
  else
  {
    sleepTime = maxNsSleep - iter.elapsed.count();
    iter.sleeptime = sleepTime;
    std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleepTime)));
  }
}
inline bool SystemFSM::step() {
  while (currentState != SystemState::Exit)
  {
    switch (currentState)
    {
    case SystemState::Initialize:
      handleInitialize();
      break;
    case SystemState::BrewSetup:
      handleBrewSetup();
      break;
    case SystemState::Brewing:
      handleBrewing();
      break;
    case SystemState::Complete:
      handleComplete();
      break;
    case SystemState::Exit:
      std::cout << "Exiting system..." << std::endl;
      break;
    default:
      std::cout << "Unknown state! Exiting for safety." << std::endl;
      currentState = SystemState::Exit;
      break;
    }
  }
}

double SystemFSM::calculateLatency(const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
  const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart) {
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart);
  return duration.count() / 1e6; // Convert nanoseconds to milliseconds
}
