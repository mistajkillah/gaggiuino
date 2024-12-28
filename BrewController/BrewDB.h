#pragma once
#include <future>
class BrewDB {
public:
    static BrewDB& getInstance();

    BrewDB(BrewDB const&) = delete;
    void operator=(BrewDB const&) = delete;

    void Initialize();
    void InsertSensorStateData(const SensorState& data);
    //void InsertSensorStateSnapshotData(const SensorStateSnapshot& data);
    std::string RetrieveLastSensorStateDataAsString() ;
    std::string RetrieveLastSensorStateSnapshotDataAsString() ;
    void StartSimulator();
    float randomFloat(float min, float max);
    SensorState generateFakeSensorState(uint32_t iteration);

    //SensorStateSnapshot generateFakeSensorStateSnapshot(uint32_t iteration);
private:
    BrewDB();
    void InitializeDatabase();
    void SimulatorThread();

    std::future<void> previousInsertFuture; // Tracks the last insertion future

    sqlite3* db;
    std::mutex dbMutex;
};

