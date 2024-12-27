#pragma once

#include <Arduino.h>
#include <array>
#include "SensorStructs.h"
//#include "sensors_state.h"
#include <stdint.h>
#include <math.h>
//#include "pindef.h"
#include <PSM.h>
//#include "utils.h"
//#include "internal_watchdog.h"

constexpr uint8_t PUMP_RANGE = 100;
#define ZC_MODE PSM::FALLING
#define TIM9 0

class Pump {
public:
    Pump(uint8_t zcPin, uint8_t dimmerPin, int powerLineFrequency, float pumpFlowAtZero);
    void Initialize();
    void setPressure(float targetPressure, float flowRestriction, const SensorState& currentState);
    void setPumpOff();
    void setPumpFullOn();
    void setPumpToRawValue(uint8_t val);
    void stopAfter(uint8_t val);

    long getAndResetClickCounter();
    int getCPS();
    void phaseShift();
    float getPumpFlow(float cps, float pressure);
    float getFlowPerClick(float pressure);
    float getClicksPerSecondForFlow(float flow, float pressure);
    void setFlow(float targetFlow, float pressureRestriction, const SensorState& currentState);

private:
    PSM pump;
    float flowPerClickAtZeroBar;
    int maxPumpClicksPerSecond;
    float fpc_multiplier;
    constexpr static std::array<float, 7> pressureInefficiencyCoefficient = {
        0.045f, 0.015f, 0.0033f, 0.000685f, 0.000045f, 0.009f, -0.0018f
    };

    float getPumpPct(float targetPressure, float flowRestriction, const SensorState& currentState);
};
