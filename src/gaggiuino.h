/* 09:32 15/03/2023 - change triggering comment */
#ifndef GAGGIUINO_H
#define GAGGIUINO_H


// Define some const values
#if defined SINGLE_BOARD
    #define GET_KTYPE_READ_EVERY    70 // max31855 amp module data read interval not recommended to be changed to lower than 70 (ms)
#else
    #define GET_KTYPE_READ_EVERY    250 // max6675 amp module data read interval not recommended to be changed to lower than 250 (ms)
#endif
#define GET_PRESSURE_READ_EVERY 10 // Pressure refresh interval (ms)
#define GET_SCALES_READ_EVERY   100 // Scales refresh interval (ms)
#define REFRESH_SCREEN_EVERY    150 // Screen refresh interval (ms)
#define REFRESH_FLOW_EVERY      50 // Flow refresh interval (ms)
#define HEALTHCHECK_EVERY       30000 // System checks happen every 30sec
#define BOILER_FILL_START_TIME  3000UL // Boiler fill start time - 3 sec since system init.
#define BOILER_FILL_TIMEOUT     8000UL // Boiler fill timeout - 8sec since system init.
#define BOILER_FILL_SKIP_TEMP   85.f // Boiler fill skip temperature threshold
#define SYS_PRESSURE_IDLE       0.7f // System pressure threshold at idle
#define MIN_WATER_LVL           10u // Min allowable tank water lvl

enum class OPERATION_MODES {
  OPMODE_straight9Bar,
  OPMODE_justPreinfusion,
  OPMODE_justPressureProfile,
  OPMODE_manual,
  OPMODE_preinfusionAndPressureProfile,
  OPMODE_flush,
  OPMODE_descale,
  OPMODE_flowPreinfusionStraight9BarProfiling,
  OPMODE_justFlowBasedProfiling,
  OPMODE_steam,
  OPMODE_FlowBasedPreinfusionPressureBasedProfiling,
  OPMODE_everythingFlowProfiled,
  OPMODE_pressureBasedPreinfusionAndFlowProfile
} ;

//Some consts
#ifndef LEGO_VALVE_RELAY
const float calibrationPressure = 2.f;
#else
const float calibrationPressure = 0.65f;
#endif

//Timers
unsigned long systemHealthTimer;
unsigned long pageRefreshTimer;
unsigned long pressureTimer;
unsigned long brewingTimer;
unsigned long thermoTimer;
unsigned long scalesTimer;
unsigned long flowTimer;
unsigned long steamTime;

//scales vars
Measurements weightMeasurements(4);

// brew detection vars
bool brewActive = false;
bool nonBrewModeActive = false;

//PP&PI variables
int preInfusionFinishedPhaseIdx = 3;
bool homeScreenScalesEnabled = false;

// Other util vars
float previousSmoothedPressure;
float previousSmoothedPumpFlow;

static void sysHealthCheck(float pressureThreshold);
static void lcdShowPopup(char *);
static void manualFlowControl();
static void lcdSetUpTime(float);
static void espCommsReadData();
static void pageValuesRefresh();
static bool sysReadinessCheck(void) ;
static void manualFlowControl(void);
static void updateProfilerPhases(void) ;
static bool isBoilerFillPhase(unsigned long elapsedTime);
static bool isSwitchOn(void);
static void readTankWaterLevel();
static void brewParamsReset();
void addPhase(PHASE_TYPE type, Transition target, float restriction, int timeMs, float pressureAbove, float pressureBelow, float shotWeight, float isWaterPumped) ;
void addFlowPhase(Transition flow, float pressureRestriction, int timeMs, float pressureAbove, float pressureBelow, float shotWeight, float isWaterPumped);
void addPressurePhase(Transition pressure, float flowRestriction, int timeMs, float pressureAbove, float pressureBelow, float shotWeight, float isWaterPumped);
void addFillBasketPhase(float flowRate);
static void profiling();
void addPreinfusionPhases();
void addSoakPhase();
void insertRampPhaseIfNeeded(size_t);
void addMainExtractionPhasesAndRamp();
float lcdGetManualFlowVol()
{
  return 0; //todo
}


enum class NextionPage {
  /* 00 */ Home,
  /* 01 */ BrewPreinfusion,
  /* 02 */ BrewSoak,
  /* 03 */ BrewProfiling,
  /* 04 */ BrewManual,
  /* 05 */ Flush,
  /* 06 */ Descale,
  /* 07 */ SettingsBoiler,
  /* 08 */ SettingsSystem,
  /* 09 */ BrewGraph,
  /* 0A */ BrewMore,
  /* 0B */ ShotSettings,
  /* 0C */ BrewTransitionProfile,
  /* 0D */ GraphPreview,
  /* 0E */ KeyboardNumeric,
  /* 0F */ Led
} ;
volatile NextionPage lcdCurrentPageId;
#endif
