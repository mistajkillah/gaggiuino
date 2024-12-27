#ifndef BREWFSM_H
#define BREWFSM_H

#include <chrono>

// Enumeration for the different states of the brewing process
enum class BrewState {
    Initialized,
    Brewing,
    Complete,
    Exit
};

// Struct to hold iteration data for the control loop
struct Iteration {
    int index;
    double sleeptime_ns;
    bool exceeds_ten;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> end;
    std::chrono::duration<double, std::nano> elapsed;
    double totalTime_ms;
};

class BrewFSM {
private:
    BrewState currentState;

    // Private member functions
    void handleInitialized();
    void handleBrewing();
    void handleComplete();
    void controlLoop();
    void executeSleep(Iteration& iter);
    double calculateLatency(const std::chrono::time_point<std::chrono::high_resolution_clock>& currStart,
                            const std::chrono::time_point<std::chrono::high_resolution_clock>& prevStart);

public:
    BrewFSM();
    void run();
    void doWork();
};

#endif // BREWFSM_H
