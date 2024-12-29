
#pragma once
#include <string>
#include <iostream>
#include <iomanip> // Required for std::put_time
#include <chrono>
#include <sstream>
#include <vector>


// static inline std::string timePointToString(const std::chrono::steady_clock::time_point& timestamp) {
//   std::ostringstream ss;
//   auto time_t_timestamp = std::chrono::steady_clock::to_time_t(timestamp);
//   auto tm_timestamp = *std::localtime(&time_t_timestamp);
//   ss << std::put_time(&tm_timestamp, "%m%d%Y:%H:%M:%S")
//     << ":" << std::setw(3) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()) % 1000).count()
//     << ":" << std::setw(6) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()) % 1000000000).count();
//   return ss.str();
// }

// PID structure to store dynamic parameters
struct PID {
  double Kp, Ki, Kd;
};

// BrewPoint structure with ramping and dynamic PID parameters
struct BrewPoint {
  double time;                 // The specific time to start this state (seconds)
  double duration;             // Duration to hold this state (seconds)
  double pressure;             // Desired pressure
  double temperature;          // Desired temperature
  PID pressurePID;             // PID parameters for pressure
  PID temperaturePID;          // PID parameters for temperature
};

// BrewProfile structure to encapsulate profile metadata and points
struct BrewProfile {
  std::string name;            // Name of the brew profile
  double StartTemp;       // Starting temperature condition
  std::vector<BrewPoint> points; // List of BrewPoints
};

static inline std::string timePointToString(const std::chrono::steady_clock::time_point& timestamp) {
    std::ostringstream ss;

    // Convert the time_point to duration since epoch
    auto duration_since_epoch = timestamp.time_since_epoch();
    
    // Break the duration into seconds and nanoseconds
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration_since_epoch).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch).count() % 1000;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch).count() % 1000000000;

    // Format the duration
    ss << seconds << ":" 
       << std::setw(3) << std::setfill('0') << milliseconds << ":"
       << std::setw(6) << std::setfill('0') << nanoseconds;

    return ss.str();
}struct SensorState {
    // Iteration and timestamps
    uint32_t iteration = 0;
    char datestamp[30] = {0};
    std::chrono::steady_clock::time_point currentReadTime_pres;
    std::chrono::steady_clock::time_point lastReadTime_pres;
    std::chrono::steady_clock::time_point currentReadTime_temp;
    std::chrono::steady_clock::time_point lastReadTime_temp;
    std::chrono::steady_clock::time_point timeSinceBrewStart;
    std::chrono::steady_clock::time_point timeSinceStartReached;
    std::chrono::steady_clock::time_point timeSinceSystemStart;

    // Time since last read
    uint32_t timeSinceLastRead_pres_ms = 0;
    float timeSinceLastRead_pres_s = 0;
    uint32_t timeSinceLastRead_temp_ms = 0;
    float timeSinceLastRead_temp_s = 0;

    // Switch states
    bool brewSwitchState = false;
    bool steamSwitchState = false;
    bool hotWaterSwitchState = false;

    // Temperature and pressure readings
    float temperature = 0.0f;
    float targetWaterTemperature = 0.0f;
    float pressure = 0.0f;
    float smoothedPressure = 0.0f;
    float previousSmoothedPressure = 0.0f;
    unsigned int brewCtrl_currentBrewProfileIndex=0;
    float smoothedPumpFlow = 0.0f;
    float previousSmoothedPumpFlow = 0.0f;
    float pressureChangeSpeed = 0.0f;
    float pumpFlowChangeSpeed = 0.0f;
    // Pump flow and other variables
    float pumpFlow = 0.0f;
    float consideredFlow = 0.0f;
    long pumpClicks = 0;

    // Heater state
    bool brewCtrl_heater_state = false;
    std::chrono::steady_clock::time_point heater_pwm_start_time;

    // Brewing control variables
    double brewCtrl_elapsed_time_sec = 0.0;
    double brewCtrl_pwm_elapsed_time_sec = 0.0;

    double brewCtrl_error_temp = 0.0;
    double brewCtrl_derivative_temp = 0.0;

    double brewCtrl_error_pressure = 0.0;
    double brewCtrl_derivative_pressure = 0.0;

    double brewCtrl_temp_output = 0.0;
    double brewCtrl_pressure_output = 0.0;
    double brewCtrl_pwm_duty_cycle = 0.0;

    BrewPoint brewCtrl_target; // Current brew target from profile

    // PID integral and previous error terms
    double integral_temp = 0.0;
    double prev_error_temp = 0.0;
    double integral_pressure = 0.0;
    double prev_error_pressure = 0.0;

    // Brew control state
    bool StartBrewingwithValveOpen = false;

    // Utility methods for time and updates
    void inline setTimeSinceSystemStart() {
        timeSinceSystemStart = std::chrono::steady_clock::now();
    }

    void inline setTimeSinceBrewStart() {
        timeSinceBrewStart = std::chrono::steady_clock::now();
    }

    void inline setTimeSinceStartReached() {
        timeSinceStartReached = std::chrono::steady_clock::now();
    }

    uint32_t inline getTimeSinceSystemStart() const {
        auto now = std::chrono::steady_clock::now();
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceSystemStart).count());
    }

    uint32_t inline getTimeSinceBrewStart() const {
        auto now = std::chrono::steady_clock::now();
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceBrewStart).count());
    }

    uint32_t inline getTimeSinceStartReached() const {
        auto now = std::chrono::steady_clock::now();
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceStartReached).count());
    }

    float inline getTimeSincelastRead_pres() {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastReadTime_pres).count());
    }

    float inline getTimeSincelastRead_temp() {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastReadTime_temp).count());
    }

    void inline updatePressureReadTime() {
        lastReadTime_pres = currentReadTime_pres;
        currentReadTime_pres = std::chrono::steady_clock::now();
        timeSinceLastRead_pres_ms = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(currentReadTime_pres - lastReadTime_pres).count());
        timeSinceLastRead_pres_s = static_cast<float>(timeSinceLastRead_pres_ms / 1000.0);
    }

    void inline updateTemperatureReadTime() {
        lastReadTime_temp = currentReadTime_temp;
        currentReadTime_temp = std::chrono::steady_clock::now();
        timeSinceLastRead_temp_ms = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(currentReadTime_temp - lastReadTime_temp).count());
        timeSinceLastRead_temp_s = static_cast<float>(timeSinceLastRead_temp_ms / 1000.0);
    }

    // Logging function
    std::string toString() const {
        std::ostringstream oss;
        oss << "Iteration: " << iteration << "\n";
        oss << "Temperature: " << temperature << " °C\n";
        oss << "Target Water Temperature: " << targetWaterTemperature << " °C\n";
        oss << "Pressure: " << pressure << " bar\n";
        oss << "Smoothed Pressure: " << smoothedPressure << " bar\n";
        oss << "Pressure Change Speed: " << pressureChangeSpeed << " bar/s\n";
        oss << "Pump Flow: " << pumpFlow << " mL/s\n";
        oss << "Considered Flow: " << consideredFlow << " mL/s\n";
        oss << "Heater State: " << (brewCtrl_heater_state ? "ON" : "OFF") << "\n";
        oss << "Elapsed Time (s): " << brewCtrl_elapsed_time_sec << "\n";
        oss << "PWM Elapsed Time (s): " << brewCtrl_pwm_elapsed_time_sec << "\n";
        oss << "Temperature Error: " << brewCtrl_error_temp << "\n";
        oss << "Temperature Derivative: " << brewCtrl_derivative_temp << "\n";
        oss << "Pressure Error: " << brewCtrl_error_pressure << "\n";
        oss << "Pressure Derivative: " << brewCtrl_derivative_pressure << "\n";
        oss << "Temperature Output: " << brewCtrl_temp_output << " %\n";
        oss << "Pressure Output: " << brewCtrl_pressure_output << " %\n";
        oss << "PWM Duty Cycle: " << brewCtrl_pwm_duty_cycle << " %\n";
        oss << "Brew Target Temperature: " << brewCtrl_target.temperature << " °C\n";
        oss << "Brew Target Pressure: " << brewCtrl_target.pressure << " bar\n";
        oss << "Start Brewing with Valve Open: " << (StartBrewingwithValveOpen ? "True" : "False") << "\n";
        return oss.str();
    }
};

struct SensorStateSnapshot
{
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


