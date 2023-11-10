#pragma once

#include <sqlite3.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <random>

struct SensorState {
    uint32_t iteration;
    char datestamp[20]; // MMDDYYYY:HH:MM:SS:MS:USEC
    uint32_t timeSinceBrewStart; // msec
    uint32_t timeSinceSystemStart; // msec
    bool brewSwitchState;
    bool steamSwitchState;
    bool hotWaterSwitchState;
    bool isSteamForgottenON;
    bool scalesPresent;
    bool tarePending;
    float temperature; // °C
    float waterTemperature; // °C
    float pressure; // bar
    float pressureChangeSpeed; // bar/s
    float pumpFlow; // ml/s
    float pumpFlowChangeSpeed; // ml/s^2
    float waterPumped;
    float weightFlow;
    float weight;
    float shotWeight;
    float smoothedPressure;
    float smoothedPumpFlow;
    float smoothedWeightFlow;
    float consideredFlow;
    long pumpClicks;
    uint16_t waterLvl;
    bool tofReady;
};

struct SensorStateSnapshot {
    uint32_t iteration;
    char datestamp[20]; // MMDDYYYY:HH:MM:SS:MS:USEC
    uint32_t timeSinceBrewStart; // msec
    uint32_t timeSinceSystemStart; // msec
    bool brewActive;
    bool steamActive;
    bool scalesPresent;
    float temperature;
    float pressure;
    float pumpFlow;
    float weightFlow;
    float weight;
    uint16_t waterLvl;
};

class BrewDBClass {
public:
    static BrewDBClass& getInstance();

    BrewDBClass(BrewDBClass const&) = delete;
    void operator=(BrewDBClass const&) = delete;

    void Initialize();
    void InsertSensorStateData(const SensorState& data);
    void InsertSensorStateSnapshotData(const SensorStateSnapshot& data);
    std::string RetrieveLastSensorStateDataAsString() ;
    std::string RetrieveLastSensorStateSnapshotDataAsString() ;
    void StartSimulator();

private:
    BrewDBClass();
    void InitializeDatabase();
    void SimulatorThread();

    sqlite3* db;
    std::mutex dbMutex;
};

