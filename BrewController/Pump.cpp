#include "Pump.h"

Pump::Pump(uint8_t zcPin, uint8_t dimmerPin, int powerLineFrequency, float pumpFlowAtZero)
    : pump(zcPin, dimmerPin, PUMP_RANGE, ZC_MODE, 1, 6) {
    maxPumpClicksPerSecond = powerLineFrequency;
    flowPerClickAtZeroBar = pumpFlowAtZero;
    fpc_multiplier = 60.f / static_cast<float>(maxPumpClicksPerSecond);
}
void Pump::Initialize()
{
  pump.Initialize();
}
float Pump::getPumpPct(float targetPressure, float flowRestriction, const SensorState& currentState) {
    if (targetPressure == 0.f) return 0.f;

    float diff = targetPressure - currentState.smoothedPressure;
    float maxPumpPct = flowRestriction <= 0.f ? 1.f : 
        getClicksPerSecondForFlow(flowRestriction, currentState.smoothedPressure) / static_cast<float>(maxPumpClicksPerSecond);
    float pumpPctToMaintainFlow = 
        getClicksPerSecondForFlow(currentState.smoothedPumpFlow, currentState.smoothedPressure) / static_cast<float>(maxPumpClicksPerSecond);

    if (diff > 2.f) {
        return fminf(maxPumpPct, 0.25f + 0.2f * diff);
    }
    if (diff > 0.f) {
        return fminf(maxPumpPct, pumpPctToMaintainFlow * 0.95f + 0.1f + 0.2f * diff);
    }
    if (currentState.pressureChangeSpeed < 0) {
        return fminf(maxPumpPct, pumpPctToMaintainFlow * 0.2f);
    }

    return 0;
}

void Pump::setPressure(float targetPressure, float flowRestriction, const SensorState& currentState) {
    float pumpPct = getPumpPct(targetPressure, flowRestriction, currentState);
    setPumpToRawValue(static_cast<uint8_t>(pumpPct * PUMP_RANGE));
}

void Pump::setPumpOff() {
    pump.set(0);
}

void Pump::setPumpFullOn() {
    pump.set(PUMP_RANGE);
}

void Pump::setPumpToRawValue(uint8_t val) {
    pump.set(val);
}

void Pump::stopAfter(uint8_t val) {
    pump.stopAfter(val);
}

long Pump::getAndResetClickCounter() {
    long counter = pump.getCounter();
    pump.resetCounter();
    return counter;
}

int Pump::getCPS() {
    //watchdogReload();
    unsigned int cps = pump.cps();
    //watchdogReload();
    if (cps > 80u) {
        pump.setDivider(2);
        pump.initTimer(cps > 110u ? 60u : 50u, TIM9);
    } else {
        pump.initTimer(cps > 55u ? 60u : 50u, TIM9);
    }
    return cps;
}

void Pump::phaseShift() {
    pump.shiftDividerCounter();
}

float Pump::getFlowPerClick(float pressure) {
    float fpc = (pressureInefficiencyCoefficient[5] / pressure + pressureInefficiencyCoefficient[6]) * (-pressure * pressure) +
                (flowPerClickAtZeroBar - pressureInefficiencyCoefficient[0]) -
                (pressureInefficiencyCoefficient[1] + 
                 (pressureInefficiencyCoefficient[2] - 
                  (pressureInefficiencyCoefficient[3] - pressureInefficiencyCoefficient[4] * pressure) * pressure) * pressure) * pressure;
    return fpc * fpc_multiplier;
}

float Pump::getPumpFlow(float cps, float pressure) {
    return cps * getFlowPerClick(pressure);
}

float Pump::getClicksPerSecondForFlow(float flow, float pressure) {
    if (flow == 0.f) return 0;
    float flowPerClick = getFlowPerClick(pressure);
    float cps = flow / flowPerClick;
    return fminf(cps, static_cast<float>(maxPumpClicksPerSecond));
}

void Pump::setFlow(float targetFlow, float pressureRestriction, const SensorState& currentState) {
    if (pressureRestriction > 0.f && currentState.smoothedPressure > pressureRestriction * 0.5f) {
        setPressure(pressureRestriction, targetFlow, currentState);
    } else {
        float pumpPct = getClicksPerSecondForFlow(targetFlow, currentState.smoothedPressure) / static_cast<float>(maxPumpClicksPerSecond);
        setPumpToRawValue(static_cast<uint8_t>(pumpPct * PUMP_RANGE));
    }
}
