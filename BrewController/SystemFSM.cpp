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
    BLOG_INFO("System is initializing...\n");
    system("rm -rf /tmp/mydb.sqlite");
    BrewDB& brewDB = BrewDB::getInstance();
    brewDB.Initialize();
    hw.initializeHW();
    currentState = SystemState::BrewSetup;
}

void SystemFSM::handleBrewSetup() {
    BLOG_ERROR("Enter brewing information (type 'start' to begin brewing): \n");
    //std::string input;
    //std::cin >> input;
    //if (input == "start") {
        currentState = SystemState::Brewing;
    //} else {
        //BLOG_ERROR("Invalid input. Try again.\n");
    //}
}

// inline void SystemFSM::handleBrewing() {
//     static int brewFSMCount = 0;
//     BLOG_DEBUG("Brewing in progress...\n");

//     SensorState state = hw.getSensorStateAsync();
//     BLOG_ERROR(state.toString().c_str());

  

//     if (brewFSMCount > TOTAL_ITERATIONS) {
//         BLOG_ERROR("Brewing complete. Transitioning to Complete state.\n");
//         currentState = SystemState::Complete;
//     }
//     brewFSMCount++;
// }

inline void SystemFSM::handleBrewing() {
    static int brewFSMCount = 0;

    // Define brewing states
    enum class BrewState { FillBoiler, Idle, Brew, Complete };
    static BrewState brewingState = BrewState::FillBoiler;

    BLOG_DEBUG("Handling Brewing FSM...\n");
            SensorState state = hw.getSensorStateAsync();
            BLOG_ERROR(state.toString().c_str());

            brewFSMCount++;
    switch (brewingState) {
        case BrewState::FillBoiler:
            BLOG_DEBUG("State: FillBoiler - Filling the boiler...\n");
            brewingState = BrewState::Idle;
            // if (hw.isBoilerFilled()) {
            //     BLOG_DEBUG("Boiler filled. Transitioning to Idle state.\n");
            //     brewingState = BrewState::Idle;
            // } else {
            //     BLOG_DEBUG("Waiting for boiler to be filled.\n");
            // }
            break;

        case BrewState::Idle:
            BLOG_DEBUG("State: Idle - Preparing for brewing...\n");
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate idle delay
            BLOG_DEBUG("Idle complete. Transitioning to Brew state.\n");
            brewingState = BrewState::Brew;
            break;

        case BrewState::Brew:
            BLOG_DEBUG("State: Brew - Brewing in progress...\n");

            if (brewFSMCount > TOTAL_ITERATIONS) {
                BLOG_DEBUG("Brewing complete. Transitioning to Complete state.\n");
                brewingState = BrewState::Complete;
            }
            break;

        case BrewState::Complete:
            BLOG_DEBUG("State: Complete - Brewing process is complete.\n");
            currentState = SystemState::Complete; // Transition to System FSM's Complete state
            brewFSMCount = 0;                     // Reset counter
            brewingState = BrewState::FillBoiler; // Reset brewing FSM for next cycle
            break;

        default:
            BLOG_ERROR("Unknown brewing state!\n");
            brewingState = BrewState::FillBoiler; // Reset to safe state
            break;
    }
}

void SystemFSM::run() {
    Iteration currentIteration = {};
    Iteration previousIteration = {};
    int currIndex = 0;
    while (currentState != SystemState::Exit) {
        std::memset(&currentIteration, 0, sizeof(Iteration));
        currentIteration.index = currIndex;
        currentIteration.start = std::chrono::high_resolution_clock::now();

        BLOG_ERROR(
            "Total Time: %f ms Execution Time: %f ms Sleep Time: %f ms\n",
            previousIteration.totalTime_ms, 
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(previousIteration.elapsed).count(),
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
                std::chrono::nanoseconds(static_cast<long long>(previousIteration.sleeptime_ns))).count());

        if (false == step()) {
            break;
        }

        currentIteration.totalTime_ms = calcualteTotalTime(currentIteration.start, previousIteration.start);
        currentIteration.end = std::chrono::high_resolution_clock::now();
        currentIteration.elapsed = currentIteration.end - currentIteration.start;
        executeSleep(currentIteration);

        previousIteration = currentIteration;
    }
}

void SystemFSM::handleComplete() {
    BLOG_DEBUG("Brewing process complete. Enjoy your beverage!\n");
    BLOG_ERROR("Type 'exit' to stop or 'restart' to initialize again: \n");
    std::string input;
    std::cin >> input;
    if (input == "exit") {
        currentState = SystemState::Exit;
    } else if (input == "restart") {
        currentState = SystemState::Initialize;
    } else {
        BLOG_ERROR("Invalid input. Staying in Complete state.\n");
    }
}


inline bool SystemFSM::step() {
    switch (currentState) {
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
            BLOG_ERROR("Exiting system...\n");
            return false;
        default:
            BLOG_ERROR("Unknown state! Exiting for safety.\n");
            currentState = SystemState::Exit;
            break;
    }
    return true;
}

double inline SystemFSM::calcualteTotalTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
    const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart) {
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart);
    return duration.count() / 1e6; // Convert nanoseconds to milliseconds
}


// inline double SystemFSM::calculateLatency(
//     const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
//     const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart) 
// {
//     return std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart).count() / 1e6; // ms
// }
inline void SystemFSM::executeSleep(Iteration& iter) {
    constexpr long maxNsSleep = 10000000; // 10ms in nanoseconds
    constexpr long minNsSleep = 1000000; // 1ms in nanoseconds
    double sleepTime = 0.0;

    if (iter.elapsed.count() > maxNsSleep) {
        iter.exceeds_ten = true;
        BLOG_ERROR("Exceeded maximum sleep time\n");
    } else {
        sleepTime = maxNsSleep - iter.elapsed.count();
        iter.sleeptime_ns = sleepTime;

        if (sleepTime >= minNsSleep) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleepTime)));
        } else {
            //BLOG_DEBUG("Skipping sleep as the calculated time is less than 1 ms\n");
        }
    }
}

inline void SystemFSM::executeSleep(const std::chrono::nanoseconds& elapsedTime) {
    constexpr long maxNsSleep = 10'000'000; // 10ms in nanoseconds
    long long sleepTime = maxNsSleep - elapsedTime.count();
    if (sleepTime > 0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
    } else {
        BLOG_DEBUG("Exceeded maximum allowed sleep time.\n");
    }
}