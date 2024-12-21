
#pragma once

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


