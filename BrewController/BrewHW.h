#pragma once

#include <vector>
#include <string>

class BrewHW {
public:
    // Constructor
    BrewHW();

    // Destructor
    ~BrewHW();

    // Initialize hardware components
    void initializeHW();

    // Sample all sensors and return their readings
    std::vector<std::string> sampleSensors();

    // Reset hardware components to their default state
    void resetHW();

    // Clean up hardware resources
    void cleanupHW();

private:
    // Private methods to sample individual sensors
    std::string sampleSensor1();
    std::string sampleSensor2();

    // Add more private methods for additional sensors as needed
};

