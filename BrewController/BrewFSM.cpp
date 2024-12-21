
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
#include "BrewController.h"
#include "SensorStructs.h"
#include "BrewFSM.h"
#include "BrewHW.h"
#include "BrewDB.h"

constexpr int TOTAL_ITERATIONS = 10;
constexpr double NANO_TO_MSEC = 1e6;

BrewFSM::BrewFSM() : currentState(BrewState::Initialized) {}

void BrewFSM::handleInitialized()
{
    std::cout << "System initialized. Ready to start brewing." << std::endl;
    std::cout << "Type 'brew' to start brewing: ";
    std::string input;
    std::cin >> input;
    if (input == "brew")
    {
        currentState = BrewState::Brewing;
    }
    else
    {
        std::cout << "Invalid input. System remains in Initialized state." << std::endl;
    }
}

void BrewFSM::handleBrewing()
{
    std::cout << "Brewing in progress..." << std::endl;
    controlLoop(); // Invoke the control loop during the brewing process
    std::cout << "Brewing complete. Transitioning to Complete state." << std::endl;
    currentState = BrewState::Complete;
}

void BrewFSM::handleComplete()
{
    std::cout << "Brewing process complete. Enjoy your beverage!" << std::endl;
    std::cout << "Type 'exit' to stop or 'restart' to initialize again: ";
    std::string input;
    std::cin >> input;
    if (input == "exit")
    {
        currentState = BrewState::Exit;
    }
    else if (input == "restart")
    {
        currentState = BrewState::Initialized;
    }
    else
    {
        std::cout << "Invalid input. Staying in Complete state." << std::endl;
    }
}

void BrewFSM::run()
{
    while (currentState != BrewState::Exit)
    {
        switch (currentState)
        {
        case BrewState::Initialized:
            handleInitialized();
            break;
        case BrewState::Brewing:
            handleBrewing();
            break;
        case BrewState::Complete:
            handleComplete();
            break;
        case BrewState::Exit:
            std::cout << "Exiting system..." << std::endl;
            break;
        default:
            std::cout << "Unknown state! Exiting for safety." << std::endl;
            currentState = BrewState::Exit;
            break;
        }
    }
}

void BrewFSM::doWork()
{
     BrewDB &db = BrewDB::getInstance();
     BrewHW &hw = BrewHW::getInstance();
     
     db.InsertSensorStateData(hw.getSensorState());
     db.InsertSensorStateSnapshotData(hw.getSensorStateSnapshot());
}

void BrewFSM::controlLoop()
{
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

        // Perform operations here
        // operation(currentIteration);

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

void BrewFSM::executeSleep(Iteration &iter)
{
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

double BrewFSM::calculateLatency(const std::chrono::time_point<std::chrono::high_resolution_clock> &currStart,
                                 const std::chrono::time_point<std::chrono::high_resolution_clock> &prevStart)
{
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currStart - prevStart);
    return duration.count() / NANO_TO_MSEC; // Convert nanoseconds to milliseconds
}
