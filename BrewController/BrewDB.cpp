#include <sstream>
#include <iostream>
#include <cstring>
#include <sqlite3.h>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <random>

#include "SensorStructs.h"
#include "BrewDB.h"
// Function to generate random float values within a given range
float randomFloat(float min, float max) {
  static std::default_random_engine generator;
  static std::uniform_real_distribution<float> distribution(min, max);
  return distribution(generator);
}


BrewDB::BrewDB() {
  int rc = sqlite3_open("/tmp/mydb.sqlite", &db);

  if (rc)
  {
    std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    exit(1);
  }
}

BrewDB& BrewDB::getInstance() {
  static BrewDB instance; // Guaranteed to be destroyed once
  return instance;
}

void BrewDB::Initialize() {
  std::lock_guard<std::mutex> lock(dbMutex);
  InitializeDatabase();
}


void BrewDB::InitializeDatabase() {
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS SensorState ("
        "iteration INTEGER, datestamp TEXT, "
        "currentReadTime_pres INTEGER, lastReadTime_pres INTEGER, "
        "currentReadTime_temp INTEGER, lastReadTime_temp INTEGER, "
        "timeSinceBrewStart INTEGER, timeSinceStartReached INTEGER, timeSinceSystemStart INTEGER, "
        "timeSinceLastRead_pres_ms INTEGER, timeSinceLastRead_pres_s REAL, "
        "timeSinceLastRead_temp_ms INTEGER, timeSinceLastRead_temp_s REAL, "
        "brewSwitchState BOOLEAN, steamSwitchState BOOLEAN, hotWaterSwitchState BOOLEAN, "
        "temperature REAL, targetWaterTemperature REAL, pressure REAL, "
        "smoothedPressure REAL, previousSmoothedPressure REAL, "
        "smoothedPumpFlow REAL, previousSmoothedPumpFlow REAL, "
        "pressureChangeSpeed REAL, pumpFlowChangeSpeed REAL, "
        "pumpFlow REAL, consideredFlow REAL, pumpClicks INTEGER, "
        "brewCtrl_heater_state BOOLEAN, brewCtrl_elapsed_time_sec REAL, "
        "brewCtrl_pwm_elapsed_time_sec REAL, "
        "brewCtrl_error_temp REAL, brewCtrl_derivative_temp REAL, "
        "brewCtrl_error_pressure REAL, brewCtrl_derivative_pressure REAL, "
        "brewCtrl_temp_output REAL, brewCtrl_pressure_output REAL, "
        "brewCtrl_pwm_duty_cycle REAL, "
        "target_time REAL, target_duration REAL, "
        "target_pressure REAL, target_temperature REAL, "
        "target_pressurePID_Kp REAL, target_pressurePID_Ki REAL, target_pressurePID_Kd REAL, "
        "target_temperaturePID_Kp REAL, target_temperaturePID_Ki REAL, target_temperaturePID_Kd REAL, "
        "StartBrewingwithValveOpen BOOLEAN)";

    int rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error creating SensorState table: " << sqlite3_errmsg(db) << std::endl;
    }
}


std::string BrewDB::RetrieveLastSensorStateDataAsString() {
    std::lock_guard<std::mutex> lock(dbMutex);

    const char* selectSQL = "SELECT * FROM SensorState ORDER BY iteration DESC LIMIT 1";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    std::string result;

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        result += "Iteration: " + std::to_string(sqlite3_column_int(stmt, 0)) + "\n";
        result += "Datestamp: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\n";
        // Add other fields as needed
    }

    sqlite3_finalize(stmt);

    return result;
}
void BrewDB::InsertSensorStateData(const SensorState& data) {
    // Wait for the previous future to finish, if it exists
    if (previousInsertFuture.valid()) {
        previousInsertFuture.wait();
    }

    // Create a new future for the current operation
    previousInsertFuture = std::async(std::launch::async, [this, data]() {
        std::lock_guard<std::mutex> lock(dbMutex);

        const char* insertSQL = "INSERT INTO SensorState (iteration, datestamp, "
                                "currentReadTime_pres, lastReadTime_pres, "
                                "currentReadTime_temp, lastReadTime_temp, "
                                "timeSinceBrewStart, timeSinceStartReached, timeSinceSystemStart, "
                                "timeSinceLastRead_pres_ms, timeSinceLastRead_pres_s, "
                                "timeSinceLastRead_temp_ms, timeSinceLastRead_temp_s, "
                                "brewSwitchState, steamSwitchState, hotWaterSwitchState, "
                                "temperature, targetWaterTemperature, pressure, "
                                "smoothedPressure, previousSmoothedPressure, "
                                "smoothedPumpFlow, previousSmoothedPumpFlow, "
                                "pressureChangeSpeed, pumpFlowChangeSpeed, "
                                "pumpFlow, consideredFlow, pumpClicks, "
                                "brewCtrl_heater_state, brewCtrl_elapsed_time_sec, "
                                "brewCtrl_pwm_elapsed_time_sec, "
                                "brewCtrl_error_temp, brewCtrl_derivative_temp, "
                                "brewCtrl_error_pressure, brewCtrl_derivative_pressure, "
                                "brewCtrl_temp_output, brewCtrl_pressure_output, "
                                "brewCtrl_pwm_duty_cycle, "
                                "target_time, target_duration, "
                                "target_pressure, target_temperature, "
                                "target_pressurePID_Kp, target_pressurePID_Ki, target_pressurePID_Kd, "
                                "target_temperaturePID_Kp, target_temperaturePID_Ki, target_temperaturePID_Kd, "
                                "StartBrewingwithValveOpen) "
                                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bind the SensorState data to the prepared statement
        sqlite3_bind_int(stmt, 1, data.iteration);
        sqlite3_bind_text(stmt, 2, data.datestamp, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, static_cast<int>(data.currentReadTime_pres.time_since_epoch().count()));
        sqlite3_bind_int(stmt, 4, static_cast<int>(data.lastReadTime_pres.time_since_epoch().count()));
        sqlite3_bind_int(stmt, 5, static_cast<int>(data.currentReadTime_temp.time_since_epoch().count()));
        sqlite3_bind_int(stmt, 6, static_cast<int>(data.lastReadTime_temp.time_since_epoch().count()));
        sqlite3_bind_int(stmt, 7, data.getTimeSinceBrewStart());
        sqlite3_bind_int(stmt, 8, data.getTimeSinceStartReached());
        sqlite3_bind_int(stmt, 9, data.getTimeSinceSystemStart());
        sqlite3_bind_int(stmt, 10, data.timeSinceLastRead_pres_ms);
        sqlite3_bind_double(stmt, 11, data.timeSinceLastRead_pres_s);
        sqlite3_bind_int(stmt, 12, data.timeSinceLastRead_temp_ms);
        sqlite3_bind_double(stmt, 13, data.timeSinceLastRead_temp_s);
        sqlite3_bind_int(stmt, 14, data.brewSwitchState);
        sqlite3_bind_int(stmt, 15, data.steamSwitchState);
        sqlite3_bind_int(stmt, 16, data.hotWaterSwitchState);
        sqlite3_bind_double(stmt, 17, data.temperature);
        sqlite3_bind_double(stmt, 18, data.targetWaterTemperature);
        sqlite3_bind_double(stmt, 19, data.pressure);
        sqlite3_bind_double(stmt, 20, data.smoothedPressure);
        sqlite3_bind_double(stmt, 21, data.previousSmoothedPressure);
        sqlite3_bind_double(stmt, 22, data.smoothedPumpFlow);
        sqlite3_bind_double(stmt, 23, data.previousSmoothedPumpFlow);
        sqlite3_bind_double(stmt, 24, data.pressureChangeSpeed);
        sqlite3_bind_double(stmt, 25, data.pumpFlowChangeSpeed);
        sqlite3_bind_double(stmt, 26, data.pumpFlow);
        sqlite3_bind_double(stmt, 27, data.consideredFlow);
        sqlite3_bind_int64(stmt, 28, data.pumpClicks);
        sqlite3_bind_int(stmt, 29, data.brewCtrl_heater_state);
        sqlite3_bind_double(stmt, 30, data.brewCtrl_elapsed_time_sec);
        sqlite3_bind_double(stmt, 31, data.brewCtrl_pwm_elapsed_time_sec);
        sqlite3_bind_double(stmt, 32, data.brewCtrl_error_temp);
        sqlite3_bind_double(stmt, 33, data.brewCtrl_derivative_temp);
        sqlite3_bind_double(stmt, 34, data.brewCtrl_error_pressure);
        sqlite3_bind_double(stmt, 35, data.brewCtrl_derivative_pressure);
        sqlite3_bind_double(stmt, 36, data.brewCtrl_temp_output);
        sqlite3_bind_double(stmt, 37, data.brewCtrl_pressure_output);
        sqlite3_bind_double(stmt, 38, data.brewCtrl_pwm_duty_cycle);

        // Bind target fields (prefixed with `target_`)
        sqlite3_bind_double(stmt, 39, data.brewCtrl_target.time);
        sqlite3_bind_double(stmt, 40, data.brewCtrl_target.duration);
        sqlite3_bind_double(stmt, 41, data.brewCtrl_target.pressure);
        sqlite3_bind_double(stmt, 42, data.brewCtrl_target.temperature);
        sqlite3_bind_double(stmt, 43, data.brewCtrl_target.pressurePID.Kp);
        sqlite3_bind_double(stmt, 44, data.brewCtrl_target.pressurePID.Ki);
        sqlite3_bind_double(stmt, 45, data.brewCtrl_target.pressurePID.Kd);
        sqlite3_bind_double(stmt, 46, data.brewCtrl_target.temperaturePID.Kp);
        sqlite3_bind_double(stmt, 47, data.brewCtrl_target.temperaturePID.Ki);
        sqlite3_bind_double(stmt, 48, data.brewCtrl_target.temperaturePID.Kd);

        sqlite3_bind_int(stmt, 49, data.StartBrewingwithValveOpen);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Error executing insert statement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
    });
}


void BrewDB::SimulatorThread() {
  uint32_t iteration = 0;
  while (true)
  {
    // Record the start time of the loop
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    SensorState sensorState;
    //SensorStateSnapshot sensorStateSnapshot;

    // Simulate data here and populate sensorState and sensorStateSnapshot

    InsertSensorStateData(generateFakeSensorState(iteration));
 
    // Insert data into SensorStateSnapshot table similarly

    // Calculate the time elapsed during loop execution
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::milliseconds elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Calculate the time remaining to sleep for approximately 1 second
    std::chrono::milliseconds sleep_time(100); // 100 msec
    std::chrono::milliseconds remaining_time = sleep_time - elapsed_time;

    if (remaining_time.count() > 0)
    {
      std::this_thread::sleep_for(remaining_time);
      iteration++;
    }
  }
}

// int main() {
//     system("rm -rf rm /tmp/mydb.sqlite");
//     BrewDBClass& brewDB = BrewDBClass::getInstance();

//     // Initialize the database and tables
//     brewDB.Initialize();

//     // Start the simulator thread
//     brewDB.StartSimulator();

//     // Keep the main thread running
//     while (true) {
//         // Add any additional functionality here
//         std::this_thread::sleep_for(std::chrono::seconds(1));

//         std::cout<<brewDB.RetrieveLastSensorStateDataAsString()<<std::endl;
//     }

//     return 0;
// }

// Function to generate random float values within a given range
float BrewDB::randomFloat(float min, float max) {
  static std::default_random_engine generator;
  static std::uniform_real_distribution<float> distribution(min, max);
  return distribution(generator);
}

// Function to generate fake data for SensorState
SensorState BrewDB::generateFakeSensorState(uint32_t iteration) {
  SensorState sensorState;
  sensorState.iteration = iteration;

  // Generate datestamp
  std::time_t now = std::time(nullptr);
  std::tm* tm_info = std::localtime(&now);
  std::ostringstream datestamp;
  datestamp << std::setw(2) << std::setfill('0') << tm_info->tm_mon + 1; // Month (1-12)
  datestamp << std::setw(2) << std::setfill('0') << tm_info->tm_mday;     // Day of the month (1-31)
  datestamp << std::setw(4) << std::setfill('0') << tm_info->tm_year + 1900; // Year
  datestamp << ":" << std::setw(2) << std::setfill('0') << tm_info->tm_hour; // Hour
  datestamp << ":" << std::setw(2) << std::setfill('0') << tm_info->tm_min;  // Minute
  datestamp << ":" << std::setw(2) << std::setfill('0') << tm_info->tm_sec;  // Second
  datestamp << ":" << std::setw(3) << std::setfill('0') << 0; // Millisecond
  datestamp << ":" << std::setw(3) << std::setfill('0') << 0; // Microsecond
  strcpy(sensorState.datestamp, datestamp.str().c_str());

  // Generate other fake data
  // sensorState.timeSinceBrewStart = randomFloat(0.0f, 60.0f); // Random value within a range
  // sensorState.timeSinceSystemStart = randomFloat(0.0f, 100.0f);
  sensorState.timeSinceBrewStart = std::chrono::steady_clock::now() - std::chrono::milliseconds(static_cast<int>(randomFloat(0.0f, 60000.0f)));
  sensorState.timeSinceSystemStart = std::chrono::steady_clock::now() - std::chrono::milliseconds(static_cast<int>(randomFloat(0.0f, 100000.0f)));

  sensorState.brewSwitchState = (randomFloat(0.0f, 1.0f) > 0.5f); // Random boolean
  sensorState.steamSwitchState = (randomFloat(0.0f, 1.0f) > 0.5f);
  sensorState.hotWaterSwitchState = (randomFloat(0.0f, 1.0f) > 0.5f);
  //sensorState.isSteamForgottenON = (randomFloat(0.0f, 1.0f) > 0.5f);
  //sensorState.scalesPresent = (randomFloat(0.0f, 1.0f) > 0.5f);
  //sensorState.tarePending = (randomFloat(0.0f, 1.0f) > 0.5f);
  sensorState.temperature = randomFloat(0.0f, 100.0f); // Random float within a range
  sensorState.targetWaterTemperature = randomFloat(0.0f, 120.0f);
  sensorState.pressure = randomFloat(0.0f, 12.0f);
  sensorState.pressureChangeSpeed = randomFloat(-10.0f, 10.0f);
  sensorState.pumpFlow = randomFloat(0.0f, 100.0f);
  sensorState.pumpFlowChangeSpeed = randomFloat(-10.0f, 10.0f);
  //sensorState.waterPumped = randomFloat(0.0f, 100.0f);
  //sensorState.weightFlow = randomFloat(0.0f, 100.0f);
  //sensorState.weight = randomFloat(0.0f, 100.0f);
  //sensorState.shotWeight = randomFloat(0.0f, 100.0f);
  sensorState.smoothedPressure = randomFloat(0.0f, 10.0f);
  sensorState.smoothedPumpFlow = randomFloat(0.0f, 100.0f);
  //sensorState.smoothedWeightFlow = randomFloat(0.0f, 100.0f);
  sensorState.consideredFlow = randomFloat(0.0f, 100.0f);
  sensorState.pumpClicks = static_cast<long>(randomFloat(0.0f, 10000.0f));
  //sensorState.waterLvl = static_cast<uint16_t>(randomFloat(0.0f, 1000.0f));
  //sensorState.tofReady = (randomFloat(0.0f, 1.0f) > 0.5f);

  return sensorState;
}
