#include "BrewFSM.h"
#include <iostream>
#include <string>

BrewFSM::BrewFSM() : currentState(BrewState::Initialized) {}

void BrewFSM::handleInitialized() {
    std::cout << "System initialized. Ready to start brewing." << std::endl;
    std::cout << "Type 'brew' to start brewing: ";
    std::string input;
    std::cin >> input;
    if (input == "brew") {
        currentState = BrewState::Brewing;
    } else {
        std::cout << "Invalid input. System remains in Initialized state." << std::endl;
    }
}

void BrewFSM::handleBrewing() {
    std::cout << "Brewing in progress..." << std::endl;
    std::cout << "Brewing complete. Transitioning to Complete state." << std::endl;
    currentState = BrewState::Complete;
}

void BrewFSM::handleComplete() {
    std::cout << "Brewing process complete. Enjoy your beverage!" << std::endl;
    std::cout << "Type 'exit' to stop or 'restart' to initialize again: ";
    std::string input;
    std::cin >> input;
    if (input == "exit") {
        currentState = BrewState::Exit;
    } else if (input == "restart") {
        currentState = BrewState::Initialized;
    } else {
        std::cout << "Invalid input. Staying in Complete state." << std::endl;
    }
}

void BrewFSM::run() {
    while (currentState != BrewState::Exit) {
        switch (currentState) {
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
