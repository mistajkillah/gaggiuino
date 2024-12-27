
#pragma once
#include <string>
#include <iostream>
#include <iomanip> // Required for std::put_time
#include <chrono>
#include <sstream>

static inline std::string timePointToString(const std::chrono::system_clock::time_point& timestamp) {
    std::ostringstream ss;
    auto time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
    auto tm_timestamp = *std::localtime(&time_t_timestamp);
    ss << std::put_time(&tm_timestamp, "%m%d%Y:%H:%M:%S")
       << ":" << std::setw(3) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()) % 1000).count()
       << ":" << std::setw(6) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()) % 1000000000).count();
    return ss.str();
}

struct SensorState {
    uint32_t iteration = 0;
    char datestamp[30] = {0};
    std::chrono::system_clock::time_point timestamp; // Native timestamp    
    uint32_t timeSinceBrewStart = 0;
    uint32_t timeSinceSystemStart = 0;
    bool brewSwitchState = false;
    bool steamSwitchState = false;
    bool hotWaterSwitchState = false;
    bool isSteamForgottenON = false;
    bool scalesPresent = false;
    bool tarePending = false;
    float temperature = 0.0f;
    float waterTemperature = 0.0f;
    float pressure = 0.0f;
    float pressureChangeSpeed = 0.0f;
    float pumpFlow = 0.0f;
    float pumpFlowChangeSpeed = 0.0f;
    float waterPumped = 0.0f;
    float weightFlow = 0.0f;
    float weight = 0.0f;
    float shotWeight = 0.0f;
    float smoothedPressure = 0.0f;
    float smoothedPumpFlow = 0.0f;
    float smoothedWeightFlow = 0.0f;
    float consideredFlow = 0.0f;
    long pumpClicks = 0;
    uint16_t waterLvl = 0;
    bool tofReady = false;

    std::string& toString()
{
    static std::string result;
    std::ostringstream oss;
    oss << "Iteration: " << iteration << "\n";
    oss << "Datestamp: " << datestamp << "\n";
    oss << "Timestamp: " << timePointToString(timestamp) << "\n";
    //oss << "Time Since Brew Start: " << timeSinceBrewStart << " ms\n";
    //oss << "Time Since System Start: " << timeSinceSystemStart << " ms\n";
    oss << "Brew Switch State: " << brewSwitchState << "\n";
    oss << "Steam Switch State: " << steamSwitchState << "\n";
    //oss << "Hot Water Switch State: " << hotWaterSwitchState << "\n";
    //oss << "Is Steam Forgotten ON: " << isSteamForgottenON << "\n";
    //oss << "Scales Present: " << scalesPresent << "\n";
    //oss << "Tare Pending: " << tarePending << "\n";
    oss << "Temperature: " << temperature << " °C\n";
    //oss << "Water Temperature: " << waterTemperature << " °C\n";
    oss << "Pressure: " << pressure << " bar\n";
    //oss << "Pressure Change Speed: " << pressureChangeSpeed << " bar/s\n";
    //oss << "Pump Flow: " << pumpFlow << " ml/s\n";
    //oss << "Pump Flow Change Speed: " << pumpFlowChangeSpeed << " ml/s²\n";
    //oss << "Water Pumped: " << waterPumped << " ml\n";
    // oss << "Weight Flow: " << weightFlow << "\n";
    // oss << "Weight: " << weight << " g\n";
    // oss << "Shot Weight: " << shotWeight << " g\n";
    // oss << "Smoothed Pressure: " << smoothedPressure << " bar\n";
    // oss << "Smoothed Pump Flow: " << smoothedPumpFlow << " ml/s\n";
    // oss << "Smoothed Weight Flow: " << smoothedWeightFlow << "\n";
    // oss << "Considered Flow: " << consideredFlow << "\n";
    // oss << "Pump Clicks: " << pumpClicks << "\n";
    // oss << "Water Level: " << waterLvl << "\n";
    // oss << "TOF Ready: " << tofReady << "\n";
    result = oss.str();
    return result;
}
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


