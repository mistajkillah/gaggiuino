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
  // Create the SensorState table
  const char* createTableSQL1 = "CREATE TABLE IF NOT EXISTS SensorState ("
    "iteration INTEGER, datestamp TEXT, timeSinceBrewStart INTEGER, timeSinceSystemStart INTEGER, "
    "brewSwitchState BOOLEAN, steamSwitchState BOOLEAN, hotWaterSwitchState BOOLEAN, isSteamForgottenON BOOLEAN, "
    "scalesPresent BOOLEAN, tarePending BOOLEAN, temperature REAL, waterTemperature REAL, pressure REAL, "
    "pressureChangeSpeed REAL, pumpFlow REAL, pumpFlowChangeSpeed REAL, waterPumped REAL, weightFlow REAL, weight REAL, "
    "shotWeight REAL, smoothedPressure REAL, smoothedPumpFlow REAL, smoothedWeightFlow REAL, consideredFlow REAL, "
    "pumpClicks INTEGER, waterLvl INTEGER, tofReady BOOLEAN)";

  

  int rc = sqlite3_exec(db, createTableSQL1, nullptr, nullptr, nullptr);
  if (rc != SQLITE_OK)
  {
    std::cerr << "Error creating SensorState table: " << sqlite3_errmsg(db) << std::endl;
  }

 
}

std::string  BrewDB::RetrieveLastSensorStateDataAsString() {
  std::lock_guard<std::mutex> lock(dbMutex);

  const char* selectSQL = "SELECT * FROM SensorState ORDER BY iteration DESC LIMIT 1";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK)
  {
    std::cerr << "Error preparing select statement for SensorState: " << sqlite3_errmsg(db) << std::endl;
    return "";
  }

  std::string result;

  if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    int iteration = sqlite3_column_int(stmt, 0);
    const char* datestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int timeSinceBrewStart = sqlite3_column_int(stmt, 2);
    int timeSinceSystemStart = sqlite3_column_int(stmt, 3);
    int brewSwitchState = sqlite3_column_int(stmt, 4);
    int steamSwitchState = sqlite3_column_int(stmt, 5);
    int hotWaterSwitchState = sqlite3_column_int(stmt, 6);
    int isSteamForgottenON = sqlite3_column_int(stmt, 7);
    int scalesPresent = sqlite3_column_int(stmt, 8);
    int tarePending = sqlite3_column_int(stmt, 9);
    double temperature = sqlite3_column_double(stmt, 10);
    double waterTemperature = sqlite3_column_double(stmt, 11);
    double pressure = sqlite3_column_double(stmt, 12);
    double pressureChangeSpeed = sqlite3_column_double(stmt, 13);
    double pumpFlow = sqlite3_column_double(stmt, 14);
    double pumpFlowChangeSpeed = sqlite3_column_double(stmt, 15);
    double waterPumped = sqlite3_column_double(stmt, 16);
    double weightFlow = sqlite3_column_double(stmt, 17);
    double weight = sqlite3_column_double(stmt, 18);
    double shotWeight = sqlite3_column_double(stmt, 19);
    double smoothedPressure = sqlite3_column_double(stmt, 20);
    double smoothedPumpFlow = sqlite3_column_double(stmt, 21);
    double smoothedWeightFlow = sqlite3_column_double(stmt, 22);
    double consideredFlow = sqlite3_column_double(stmt, 23);
    int64_t pumpClicks = sqlite3_column_int64(stmt, 24);
    int waterLvl = sqlite3_column_int(stmt, 25);
    int tofReady = sqlite3_column_int(stmt, 26);

    // Append retrieved data to the result string
    result += "Iteration: " + std::to_string(iteration) + "\n";
    result += "Datestamp: " + std::string(datestamp) + "\n";
    result += "Time Since Brew Start: " + std::to_string(timeSinceBrewStart) + "\n";
    result += "Time Since System Start: " + std::to_string(timeSinceSystemStart) + "\n";
    result += "Brew Switch State: " + std::to_string(brewSwitchState) + "\n";
    result += "Steam Switch State: " + std::to_string(steamSwitchState) + "\n";
    result += "Hot Water Switch State: " + std::to_string(hotWaterSwitchState) + "\n";
    result += "Is Steam Forgotten ON: " + std::to_string(isSteamForgottenON) + "\n";
    result += "Scales Present: " + std::to_string(scalesPresent) + "\n";
    result += "Tare Pending: " + std::to_string(tarePending) + "\n";
    result += "Temperature: " + std::to_string(temperature) + "\n";
    result += "Water Temperature: " + std::to_string(waterTemperature) + "\n";
    result += "Pressure: " + std::to_string(pressure) + "\n";
    result += "Pressure Change Speed: " + std::to_string(pressureChangeSpeed) + "\n";
    result += "Pump Flow: " + std::to_string(pumpFlow) + "\n";
    result += "Pump Flow Change Speed: " + std::to_string(pumpFlowChangeSpeed) + "\n";
    result += "Water Pumped: " + std::to_string(waterPumped) + "\n";
    result += "Weight Flow: " + std::to_string(weightFlow) + "\n";
    result += "Weight: " + std::to_string(weight) + "\n";
    result += "Shot Weight: " + std::to_string(shotWeight) + "\n";
    result += "Smoothed Pressure: " + std::to_string(smoothedPressure) + "\n";
    result += "Smoothed Pump Flow: " + std::to_string(smoothedPumpFlow) + "\n";
    result += "Smoothed Weight Flow: " + std::to_string(smoothedWeightFlow) + "\n";
    result += "Considered Flow: " + std::to_string(consideredFlow) + "\n";
    result += "Pump Clicks: " + std::to_string(pumpClicks) + "\n";
    result += "Water Level: " + std::to_string(waterLvl) + "\n";
    result += "TOF Ready: " + std::to_string(tofReady) + "\n";
  }

  sqlite3_finalize(stmt);

  return result;
}

// Function to retrieve the last entry from the SensorStateSnapshot table and generate a string
std::string BrewDB::RetrieveLastSensorStateSnapshotDataAsString() {
  std::lock_guard<std::mutex> lock(dbMutex);

  const char* selectSQL = "SELECT * FROM SensorStateSnapshot ORDER BY iteration DESC LIMIT 1";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

  if (rc != SQLITE_OK)
  {
    std::cerr << "Error preparing select statement for SensorStateSnapshot: " << sqlite3_errmsg(db) << std::endl;
    return "";
  }

  std::string result;

  if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    int iteration = sqlite3_column_int(stmt, 0);
    const char* datestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int timeSinceBrewStart = sqlite3_column_int(stmt, 2);
    int timeSinceSystemStart = sqlite3_column_int(stmt, 3);
    int brewActive = sqlite3_column_int(stmt, 4);
    int steamActive = sqlite3_column_int(stmt, 5);
    int scalesPresent = sqlite3_column_int(stmt, 6);
    double temperature = sqlite3_column_double(stmt, 7);
    double pressure = sqlite3_column_double(stmt, 8);
    double pumpFlow = sqlite3_column_double(stmt, 9);
    double weightFlow = sqlite3_column_double(stmt, 10);
    double weight = sqlite3_column_double(stmt, 11);
    int waterLvl = sqlite3_column_int(stmt, 12);

    // Append retrieved data to the result string
    result += "Iteration: " + std::to_string(iteration) + "\n";
    result += "Datestamp: " + std::string(datestamp) + "\n";
    result += "Time Since Brew Start: " + std::to_string(timeSinceBrewStart) + "\n";
    result += "Time Since System Start: " + std::to_string(timeSinceSystemStart) + "\n";
    result += "Brew Active: " + std::to_string(brewActive) + "\n";
    result += "Steam Active: " + std::to_string(steamActive) + "\n";
    result += "Scales Present: " + std::to_string(scalesPresent) + "\n";
    result += "Temperature: " + std::to_string(temperature) + "\n";
    result += "Pressure: " + std::to_string(pressure) + "\n";
    result += "Pump Flow: " + std::to_string(pumpFlow) + "\n";
    result += "Weight Flow: " + std::to_string(weightFlow) + "\n";
    result += "Weight: " + std::to_string(weight) + "\n";
    result += "Water Level: " + std::to_string(waterLvl) + "\n";
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

        const char* insertSQL = "INSERT INTO SensorState (iteration, datestamp, timeSinceBrewStart, timeSinceSystemStart, "
                                "brewSwitchState, steamSwitchState, hotWaterSwitchState, isSteamForgottenON, scalesPresent, tarePending, "
                                "temperature, waterTemperature, pressure, pressureChangeSpeed, pumpFlow, pumpFlowChangeSpeed, waterPumped, "
                                "weightFlow, weight, shotWeight, smoothedPressure, smoothedPumpFlow, smoothedWeightFlow, consideredFlow, "
                                "pumpClicks, waterLvl, tofReady) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Error preparing insert statement for SensorState: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bind values to the prepared statement
        sqlite3_bind_int(stmt, 1, data.iteration);
        sqlite3_bind_text(stmt, 2, data.datestamp, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(data.timeSinceBrewStart.time_since_epoch()).count()
        ));
        sqlite3_bind_int(stmt, 4, static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(data.timeSinceSystemStart.time_since_epoch()).count()
        ));
        sqlite3_bind_int(stmt, 5, data.brewSwitchState);
        sqlite3_bind_int(stmt, 6, data.steamSwitchState);
        sqlite3_bind_int(stmt, 7, data.hotWaterSwitchState);
        sqlite3_bind_int(stmt, 8, data.isSteamForgottenON);
        sqlite3_bind_int(stmt, 9, data.scalesPresent);
        sqlite3_bind_int(stmt, 10, data.tarePending);
        sqlite3_bind_double(stmt, 11, data.temperature);
        sqlite3_bind_double(stmt, 12, data.targetWaterTemperature);
        sqlite3_bind_double(stmt, 13, data.pressure);
        sqlite3_bind_double(stmt, 14, data.pressureChangeSpeed);
        sqlite3_bind_double(stmt, 15, data.pumpFlow);
        sqlite3_bind_double(stmt, 16, data.pumpFlowChangeSpeed);
        sqlite3_bind_double(stmt, 17, data.waterPumped);
        sqlite3_bind_double(stmt, 18, data.weightFlow);
        sqlite3_bind_double(stmt, 19, data.weight);
        sqlite3_bind_double(stmt, 20, data.shotWeight);
        sqlite3_bind_double(stmt, 21, data.smoothedPressure);
        sqlite3_bind_double(stmt, 22, data.smoothedPumpFlow);
        sqlite3_bind_double(stmt, 23, data.smoothedWeightFlow);
        sqlite3_bind_double(stmt, 24, data.consideredFlow);
        sqlite3_bind_int64(stmt, 25, data.pumpClicks);
        sqlite3_bind_int(stmt, 26, data.waterLvl);
        sqlite3_bind_int(stmt, 27, data.tofReady);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Error executing insert statement for SensorState: " << sqlite3_errmsg(db) << std::endl;
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
  sensorState.isSteamForgottenON = (randomFloat(0.0f, 1.0f) > 0.5f);
  sensorState.scalesPresent = (randomFloat(0.0f, 1.0f) > 0.5f);
  sensorState.tarePending = (randomFloat(0.0f, 1.0f) > 0.5f);
  sensorState.temperature = randomFloat(0.0f, 100.0f); // Random float within a range
  sensorState.targetWaterTemperature = randomFloat(0.0f, 120.0f);
  sensorState.pressure = randomFloat(0.0f, 12.0f);
  sensorState.pressureChangeSpeed = randomFloat(-10.0f, 10.0f);
  sensorState.pumpFlow = randomFloat(0.0f, 100.0f);
  sensorState.pumpFlowChangeSpeed = randomFloat(-10.0f, 10.0f);
  sensorState.waterPumped = randomFloat(0.0f, 100.0f);
  sensorState.weightFlow = randomFloat(0.0f, 100.0f);
  sensorState.weight = randomFloat(0.0f, 100.0f);
  sensorState.shotWeight = randomFloat(0.0f, 100.0f);
  sensorState.smoothedPressure = randomFloat(0.0f, 10.0f);
  sensorState.smoothedPumpFlow = randomFloat(0.0f, 100.0f);
  sensorState.smoothedWeightFlow = randomFloat(0.0f, 100.0f);
  sensorState.consideredFlow = randomFloat(0.0f, 100.0f);
  sensorState.pumpClicks = static_cast<long>(randomFloat(0.0f, 10000.0f));
  sensorState.waterLvl = static_cast<uint16_t>(randomFloat(0.0f, 1000.0f));
  sensorState.tofReady = (randomFloat(0.0f, 1.0f) > 0.5f);

  return sensorState;
}
