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
    BLOG_INFO("System is initializing...");
    system("rm -rf /tmp/mydb.sqlite");
    BrewDB& brewDB = BrewDB::getInstance();
    brewDB.Initialize();
    hw.initializeHW();
    currentState = SystemState::BrewSetup;
}

void SystemFSM::handleBrewSetup() {
    BLOG_ERROR("Enter brewing information (type 'start' to begin brewing): ");
    std::string input;
    std::cin >> input;
    if (input == "start") {
        currentState = SystemState::Brewing;
    } else {
        BLOG_ERROR("Invalid input. Try again.");
    }
}

inline void SystemFSM::handleBrewing() {
    static int brewFSMCount = 0;
    BLOG_DEBUG("Brewing in progress...");

    SensorState state = hw.getSensorState();
    BLOG_ERROR(state.toString().c_str());

    if (brewFSMCount > TOTAL_ITERATIONS) {
        BLOG_ERROR("Brewing complete. Transitioning to Complete state.");
        currentState = SystemState::Complete;
    }
    brewFSMCount++;
}

void SystemFSM::run() {
    Iteration currentIteration = {};
    Iteration previousIteration = {};
    int currIndex = 0;
    while (currentState != SystemState::Exit) {
        std::memset(&currentIteration, 0, sizeof(Iteration));
        currentIteration.index = currIndex;
        currentIteration.start = std::chrono::high_resolution_clock::now();

        BLOG_ERROR("Previous iteration latency: %f ms", previousIteration.latency);

        if (false == step()) {
            break;
        }

        currentIteration.latency = calculateLatency(currentIteration.start, previousIteration.start);
        currentIteration.end = std::chrono::high_resolution_clock::now();
        currentIteration.elapsed = currentIteration.end - currentIteration.start;
        executeSleep(currentIteration);

        previousIteration = currentIteration;
    }
}

void SystemFSM::handleComplete() {
    BLOG_DEBUG("Brewing process complete. Enjoy your beverage!");
    BLOG_ERROR("Type 'exit' to stop or 'restart' to initialize again: ");
    std::string input;
    std::cin >> input;
    if (input == "exit") {
        currentState = SystemState::Exit;
    } else if (input == "restart") {
        currentState = SystemState::Initialize;
    } else {
        BLOG_ERROR("Invalid input. Staying in Complete state.");
    }
}

inline void SystemFSM::executeSleep(Iteration& iter) {
    constexpr long maxNsSleep = 10000000; // 10ms
    double sleepTime = 0.0;
    if (iter.elapsed.count() > maxNsSleep) {
        iter.exceeds_ten = true;
        BLOG_ERROR("Exceeded maximum sleep time");
    } else {
        sleepTime = maxNsSleep - iter.elapsed.count();
        iter.sleeptime = sleepTime;
        std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleepTime)));
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
            BLOG_ERROR("Exiting system...");
            return false;
        default:
            BLOG_ERROR("Unknown state! Exiting for safety.");
            currentState = SystemState::Exit;
            break;
    }
    return true;
}

double inline SystemFSM::calculateLatency(const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
    const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart) {
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart);
    return duration.count() / 1e6; // Convert nanoseconds to milliseconds
}
