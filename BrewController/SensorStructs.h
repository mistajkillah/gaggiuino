
#pragma once
#include <string>
struct SensorState {
    uint32_t iteration;
    char datestamp[30]; // MMDDYYYY:HH:MM:SS:MS:USEC
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

    std::string& toString()
{
    static std::string result;
    std::ostringstream oss;
    oss << "Iteration: " << iteration << "\n";
    oss << "Datestamp: " << datestamp << "\n";
    oss << "Time Since Brew Start: " << timeSinceBrewStart << " ms\n";
    oss << "Time Since System Start: " << timeSinceSystemStart << " ms\n";
    oss << "Brew Switch State: " << brewSwitchState << "\n";
    oss << "Steam Switch State: " << steamSwitchState << "\n";
    oss << "Hot Water Switch State: " << hotWaterSwitchState << "\n";
    oss << "Is Steam Forgotten ON: " << isSteamForgottenON << "\n";
    oss << "Scales Present: " << scalesPresent << "\n";
    oss << "Tare Pending: " << tarePending << "\n";
    oss << "Temperature: " << temperature << " °C\n";
    oss << "Water Temperature: " << waterTemperature << " °C\n";
    oss << "Pressure: " << pressure << " bar\n";
    oss << "Pressure Change Speed: " << pressureChangeSpeed << " bar/s\n";
    oss << "Pump Flow: " << pumpFlow << " ml/s\n";
    oss << "Pump Flow Change Speed: " << pumpFlowChangeSpeed << " ml/s²\n";
    oss << "Water Pumped: " << waterPumped << " ml\n";
    oss << "Weight Flow: " << weightFlow << "\n";
    oss << "Weight: " << weight << " g\n";
    oss << "Shot Weight: " << shotWeight << " g\n";
    oss << "Smoothed Pressure: " << smoothedPressure << " bar\n";
    oss << "Smoothed Pump Flow: " << smoothedPumpFlow << " ml/s\n";
    oss << "Smoothed Weight Flow: " << smoothedWeightFlow << "\n";
    oss << "Considered Flow: " << consideredFlow << "\n";
    oss << "Pump Clicks: " << pumpClicks << "\n";
    oss << "Water Level: " << waterLvl << "\n";
    oss << "TOF Ready: " << tofReady << "\n";
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


