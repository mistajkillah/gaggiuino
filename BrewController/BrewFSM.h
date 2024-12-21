#pragma once
#include <string>

enum class BrewState {
    Initialized,
    Brewing,
    Complete,
    Exit
};

class BrewFSM {
private:
    BrewState currentState;

    // Private helper functions for each state
    void handleInitialized();
    void handleBrewing();
    void handleComplete();

public:
    BrewFSM();
    void run();
};
