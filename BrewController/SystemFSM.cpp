#include "BrewHW.h"
#include "BrewFSM.h"
#include "SystemFSM.h"
#include <iostream>

SystemFSM::SystemFSM() : hw(*(new BrewHW())), brewFSM(*(new BrewFSM())) ,currentState(SystemState::Initialize) 
{

}

void SystemFSM::handleInitialize() {
    std::cout << "System is initializing..." << std::endl;
    // Transition to BrewSetup after initialization
    currentState = SystemState::BrewSetup;
}

void SystemFSM::handleBrewSetup() {
    std::cout << "Enter brewing information (type 'start' to begin brewing): ";
    std::string input;
    std::cin >> input;
    if (input == "start") {
        currentState = SystemState::Brew;
    } else {
        std::cout << "Invalid input. Try again." << std::endl;
    }
}

void SystemFSM::handleBrew() {
    std::cout << "Brewing process started..." << std::endl;
    // Transition to ControlLoop after brewing starts
    currentState = SystemState::BrewSetup;
}

void SystemFSM::handleControlLoop() {
    std::cout << "Running control loop..." << std::endl;
    std::cout << "Type 'exit' to stop or any key to continue: ";
    std::string input;
    std::cin >> input;
    if (input == "exit") {
        currentState = SystemState::Exit;
    } else {
        std::cout << "Continuing control loop..." << std::endl;
    }
}

void SystemFSM::run() {
    while (currentState != SystemState::Exit) {
        switch (currentState) {
            case SystemState::Initialize:
                handleInitialize();
                break;
            case SystemState::BrewSetup:
                handleBrewSetup();
                break;
            case SystemState::Brew:
                handleBrew();
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
