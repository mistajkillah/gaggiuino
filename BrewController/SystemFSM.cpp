#include <iostream>
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
#include "SensorStructs.h"
#include "BrewHW.h"
#include "BrewFSM.h"
#include "BrewDB.h"

#include "SystemFSM.h"



SystemFSM::SystemFSM() : hw(BrewHW::getInstance()), brewFSM(*(new BrewFSM())) ,currentState(SystemState::Initialize) 
{

}

void SystemFSM::handleInitialize() {
    std::cout << "System is initializing..." << std::endl;
    // Transition to BrewSetup after initialization

    system("rm -rf rm /tmp/mydb.sqlite");
    BrewDB& brewDB = BrewDB::getInstance();
   
    //Initialize the database and tables
    brewDB.Initialize();
    hw.initializeHW();
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
    brewFSM.run();
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
