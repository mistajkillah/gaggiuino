#pragma once

#include <string>

enum class SystemState {
    Initialize,
    BrewSetup,
    Brew,
    Exit
};
class BrewHW;
class BrewFSM;

class SystemFSM {
private:
    SystemState currentState;
    BrewHW &hw;
    BrewFSM &brewFSM;
    // Private helper functions for each state
    void handleInitialize();
    void handleBrewSetup();
    void handleBrew();
    void handleControlLoop();

public:
    SystemFSM();
    void run();
};

