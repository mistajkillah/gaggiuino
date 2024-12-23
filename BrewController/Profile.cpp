#include <stdio.h>
#include "Profile.h"

static BrewConfig runningCfg;

  BrewConfig getEepromDefaults(void) {
    BrewConfig defaultData;

    // Profiles
    defaultData.activeProfile = 0;
    for (int i=0; i<MAX_PROFILES; i++) {
      snprintf(defaultData.profiles[i].name, PROFILE_NAME_LENGTH, "%s", defaultsProfile[i].name);
      // temp

      // PI
      defaultData.profiles[i].preinfusionState = defaultsProfile[i].preinfusionState;
      defaultData.profiles[i].preinfusionSec = defaultsProfile[i].preinfusionSec;
      defaultData.profiles[i].preinfusionBar = defaultsProfile[i].preinfusionBar;
      defaultData.profiles[i].preinfusionRamp = defaultsProfile[i].preinfusionRamp;
      defaultData.profiles[i].preinfusionFlowState = defaultsProfile[i].preinfusionFlowState;
      defaultData.profiles[i].preinfusionFlowVol = defaultsProfile[i].preinfusionFlowVol;
      defaultData.profiles[i].preinfusionFlowTime = defaultsProfile[i].preinfusionFlowTime;
      defaultData.profiles[i].preinfusionFlowPressureTarget = defaultsProfile[i].preinfusionFlowPressureTarget;
      defaultData.profiles[i].preinfusionPressureFlowTarget = defaultsProfile[i].preinfusionPressureFlowTarget;
      defaultData.profiles[i].preinfusionFilled = defaultsProfile[i].preinfusionFilled;
      defaultData.profiles[i].preinfusionPressureAbove = defaultsProfile[i].preinfusionPressureAbove;
      defaultData.profiles[i].preinfusionWeightAbove = defaultsProfile[i].preinfusionWeightAbove;
      // SOAK
      defaultData.profiles[i].soakState = defaultsProfile[i].soakState;
      defaultData.profiles[i].soakTimePressure = defaultsProfile[i].soakTimePressure;
      defaultData.profiles[i].soakTimeFlow = defaultsProfile[i].soakTimeFlow;
      defaultData.profiles[i].soakKeepPressure = defaultsProfile[i].soakKeepPressure;
      defaultData.profiles[i].soakKeepFlow = defaultsProfile[i].soakKeepFlow;
      defaultData.profiles[i].soakBelowPressure = defaultsProfile[i].soakBelowPressure;
      defaultData.profiles[i].soakAbovePressure = defaultsProfile[i].soakAbovePressure;
      defaultData.profiles[i].soakAboveWeight = defaultsProfile[i].soakAboveWeight;
      // PI -> TP/PF
      defaultData.profiles[i].preinfusionRamp = defaultsProfile[i].preinfusionRamp;
      defaultData.profiles[i].preinfusionRampSlope = defaultsProfile[i].preinfusionRampSlope;
      // Transition Profile - ramp&hold || advanced profiling
      defaultData.profiles[i].tpState = defaultsProfile[i].tpState;
      defaultData.profiles[i].tpType = defaultsProfile[i].tpType; // transtion profile type :pressure|flow:
      defaultData.profiles[i].tpProfilingStart = defaultsProfile[i].tpProfilingStart;
      defaultData.profiles[i].tpProfilingFinish = defaultsProfile[i].tpProfilingFinish;
      defaultData.profiles[i].tpProfilingHold = defaultsProfile[i].tpProfilingHold;
      defaultData.profiles[i].tpProfilingHoldLimit = defaultsProfile[i].tpProfilingHoldLimit;
      defaultData.profiles[i].tpProfilingSlope = defaultsProfile[i].tpProfilingSlope;
      defaultData.profiles[i].tpProfilingSlopeShape = defaultsProfile[i].tpProfilingSlopeShape;
      defaultData.profiles[i].tpProfilingFlowRestriction = defaultsProfile[i].tpProfilingFlowRestriction;
      defaultData.profiles[i].tfProfileStart = defaultsProfile[i].tfProfileStart;
      defaultData.profiles[i].tfProfileEnd = defaultsProfile[i].tfProfileEnd;
      defaultData.profiles[i].tfProfileHold = defaultsProfile[i].tfProfileHold;
      defaultData.profiles[i].tfProfileHoldLimit = defaultsProfile[i].tfProfileHoldLimit;
      defaultData.profiles[i].tfProfileSlope = defaultsProfile[i].tfProfileSlope;
      defaultData.profiles[i].tfProfileSlopeShape = defaultsProfile[i].tfProfileSlopeShape;
      defaultData.profiles[i].tfProfilingPressureRestriction = defaultsProfile[i].tfProfilingPressureRestriction;
      // Profiling
      defaultData.profiles[i].profilingState = defaultsProfile[i].profilingState;
      defaultData.profiles[i].mfProfileState = defaultsProfile[i].mfProfileState;
      defaultData.profiles[i].mpProfilingStart = defaultsProfile[i].mpProfilingStart;
      defaultData.profiles[i].mpProfilingFinish = defaultsProfile[i].mpProfilingFinish;
      defaultData.profiles[i].mpProfilingSlope = defaultsProfile[i].mpProfilingSlope;
      defaultData.profiles[i].mpProfilingSlopeShape = defaultsProfile[i].mpProfilingSlopeShape;
      defaultData.profiles[i].mpProfilingFlowRestriction = defaultsProfile[i].mpProfilingFlowRestriction;
      defaultData.profiles[i].mfProfileStart = defaultsProfile[i].mfProfileStart;
      defaultData.profiles[i].mfProfileEnd = defaultsProfile[i].mfProfileEnd;
      defaultData.profiles[i].mfProfileSlope = defaultsProfile[i].mfProfileSlope;
      defaultData.profiles[i].mfProfileSlopeShape = defaultsProfile[i].mfProfileSlopeShape;
      defaultData.profiles[i].mfProfilingPressureRestriction = defaultsProfile[i].mfProfilingPressureRestriction;
      /*-----------------------OTHER-----------------*/
      defaultData.profiles[i].setpoint = defaultsProfile[i].setpoint;
      // Dose settings
      defaultData.profiles[i].stopOnWeightState = defaultsProfile[i].stopOnWeightState;
      defaultData.profiles[i].shotDose = defaultsProfile[i].shotDose;
      defaultData.profiles[i].shotStopOnCustomWeight = defaultsProfile[i].shotStopOnCustomWeight;
      defaultData.profiles[i].shotPreset = defaultsProfile[i].shotPreset;
    }
    // General brew settings
    defaultData.homeOnShotFinish = false;
    defaultData.brewDeltaState = true;
    defaultData.basketPrefill = false;
    // System settings
    defaultData.steamSetPoint = 155;
    defaultData.offsetTemp = 7;
    defaultData.hpwr = 550;
    defaultData.mainDivider = 5;
    defaultData.brewDivider = 3;
    defaultData.powerLineFrequency = 50;
    defaultData.lcdSleep = 16;
    defaultData.warmupState = false;
    defaultData.scalesF1 = 3920;
    defaultData.scalesF2 = 4210;
    defaultData.pumpFlowAtZero = 0.2225f;
    defaultData.ledState  = true;
    defaultData.ledDisco  = true;
    defaultData.ledR = 9;
    defaultData.ledG = 0;
    defaultData.ledB = 9;

    return defaultData;
  }

  // kind of annoying, but allows reusing macro without messing up type safety
  template <typename T>
  bool copy_t(T& target, T& source) {
    target = source;
    return true;
  }

  //bool loadCurrentEepromData EEPROM_METADATA_LOADER(EEPROM_DATA_VERSION, eepromMetadata_t, copy_t);

}