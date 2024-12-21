#pragma once



class BrewHW {
public:
    // Deleted copy constructor and assignment operator to prevent copying
    BrewHW(const BrewHW&) = delete;
    BrewHW& operator=(const BrewHW&) = delete;

    // Static method to access the singleton instance
    static BrewHW& getInstance();

    // Public methods
    int initializeHW();
    
    SensorState getSensorState();
    SensorStateSnapshot getSensorStateSnapshot();
    void resetHW();
    void cleanupHW();

    // GPIO pin constants
    static const int inputPin = 22;  // PIN 15 interrupt pin
    static const int outputPin = 27; // PIN 13 response to interrupt pin
    static const int togglePin = 17; // PIN 11 fake clock/interrupt aggressor

private:
    // Private constructor and destructor
    BrewHW();
    ~BrewHW();
    SensorState currentState;
    // Private methods to sample individual sensors
    std::string sampleSensor1();
    std::string sampleSensor2();

    // Static method for input callback
    static void inputCallback(int gpio, int level, uint32_t tick);
    void setBoilerOn(void);
    void setBoilerOff(void);
    void setSteamValveRelayOn(void);
    void setSteamValveRelayOff(void);
    void setSteamBoilerRelayOn(void);
    void setSteamBoilerRelayOff(void);
    int brewState(void);
    int steamState(void);
    int waterPinState(void);
    void openValve(void);
    void closeValve(void);
    void sensorReadSwitches(void);
    void sensorsReadTemperature(void);

        //Timers
    unsigned long systemHealthTimer;
    unsigned long pageRefreshTimer;
    unsigned long pressureTimer;
    unsigned long brewingTimer;
    unsigned long thermoTimer;
    unsigned long scalesTimer;
    unsigned long flowTimer;
    unsigned long steamTime;

};
