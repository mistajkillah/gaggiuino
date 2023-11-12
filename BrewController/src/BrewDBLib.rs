#[allow(non_snake_case)]
extern crate chrono;
extern crate rand;
extern crate rusqlite;
//use std::time::Duration;
use chrono::{Local, Timelike};


use rand::Rng;
use rusqlite::{params, Connection, Result};
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::{Duration, SystemTime};


pub struct SensorState {
    pub iteration: u32,
    pub datestamp: String,
    pub time_since_brew_start: u32,
    pub time_since_system_start: u32,
    pub brew_switch_state: bool,
    pub steam_switch_state: bool,
    pub hot_water_switch_state: bool,
    pub is_steam_forgotten_on: bool,
    pub scales_present: bool,
    pub tare_pending: bool,
    pub temperature: f32,
    pub water_temperature: f32,
    pub pressure: f32,
    pub pressure_change_speed: f32,
    pub pump_flow: f32,
    pub pump_flow_change_speed: f32,
    pub water_pumped: f32,
    pub weight_flow: f32,
    pub weight: f32,
    pub shot_weight: f32,
    pub smoothed_pressure: f32,
    pub smoothed_pump_flow: f32,
    pub smoothed_weight_flow: f32,
    pub considered_flow: f32,
    pub pump_clicks: i64,
    pub water_lvl: u16,
    pub tof_ready: bool,
}

pub struct SensorStateSnapshot {
    pub iteration: u32,
    pub datestamp: String,
    pub time_since_brew_start: u32,
    pub time_since_system_start: u32,
    pub brew_active: bool,
    pub steam_active: bool,
    pub scales_present: bool,
    pub temperature: f32,
    pub pressure: f32,
    pub pump_flow: f32,
    pub weight_flow: f32,
    pub weight: f32,
    pub water_lvl: u16,
}
impl SensorState {
    // Add a public method to set the datestamp
    pub fn set_datestamp(&mut self, datestamp: String) {
        self.datestamp = datestamp;
    }
}

impl SensorStateSnapshot {
    // Add a public method to set the datestamp
    pub fn set_datestamp(&mut self, datestamp: String) {
        self.datestamp = datestamp;
    }
}



fn random_float(min: f32, max: f32) -> f32 {
    let mut rng = rand::thread_rng();
    rng.gen_range(min..max)
}

fn generate_fake_sensor_state(iteration: u32) -> SensorState {
    let mut sensor_state = SensorState {
        iteration,
        datestamp: String::new(),
        time_since_brew_start: random_float(0.0, 60.0) as u32,        
        time_since_system_start: random_float(0.0, 60.0) as u32,   
        brew_switch_state: random_float(0.0, 1.0) > 0.5,
        steam_switch_state: random_float(0.0, 1.0) > 0.5,
        hot_water_switch_state: random_float(0.0, 1.0) > 0.5,
        is_steam_forgotten_on: random_float(0.0, 1.0) > 0.5,
        scales_present: random_float(0.0, 1.0) > 0.5,
        tare_pending: random_float(0.0, 1.0) > 0.5,
        temperature: random_float(0.0, 100.0),
        water_temperature: random_float(0.0, 120.0),
        pressure: random_float(0.0, 12.0),
        pressure_change_speed: random_float(-10.0, 10.0),
        pump_flow: random_float(0.0, 100.0),
        pump_flow_change_speed: random_float(-10.0, 10.0),
        water_pumped: random_float(0.0, 100.0),
        weight_flow: random_float(0.0, 100.0),
        weight: random_float(0.0, 100.0),
        shot_weight: random_float(0.0, 100.0),
        smoothed_pressure: random_float(0.0, 10.0),
        smoothed_pump_flow: random_float(0.0, 100.0),
        smoothed_weight_flow: random_float(0.0, 100.0),
        considered_flow: random_float(0.0, 100.0),
        pump_clicks: random_float(0.0, 10000.0) as i64,
        water_lvl: random_float(0.0, 1000.0) as u16,
        tof_ready: random_float(0.0, 1.0) > 0.5,
    };

    // Generate datestamp (similar to C++ code)
    let now = SystemTime::now();
    let tm_info = now.duration_since(SystemTime::UNIX_EPOCH).unwrap().as_secs();
    let datestamp = format!(
        "{:02}{:02}{:04}:{:02}:{:02}:{:02}:{:03}:{:03}",
        (tm_info % 12) + 1,
        (tm_info % 31) + 1,
        (tm_info / 31 / 12) + 1900,
        (tm_info / 3600) % 24,
        (tm_info / 60) % 60,
        tm_info % 60,
        0, // Millisecond
        0  // Microsecond
    );

    sensor_state.set_datestamp(datestamp);

    sensor_state
}

fn generate_fake_sensor_state_snapshot(iteration: u32) -> SensorStateSnapshot {
    let mut snapshot = SensorStateSnapshot {
        iteration,
        datestamp: String::new(),
        time_since_brew_start: random_float(0.0, 60.0) as u32,        
        time_since_system_start: random_float(0.0, 60.0) as u32,   
        brew_active: random_float(0.0, 1.0) > 0.5,
        steam_active: random_float(0.0, 1.0) > 0.5,
        scales_present: random_float(0.0, 1.0) > 0.5,
        temperature: random_float(0.0, 100.0),
        pressure: random_float(0.0, 10.0),
        pump_flow: random_float(0.0, 100.0),
        weight_flow: random_float(0.0, 100.0),
        weight: random_float(0.0, 1000.0),
        water_lvl: random_float(0.0, 1000.0) as u16,
    };

    // Generate datestamp (similar to C++ code)
    let now = SystemTime::now();
    let tm_info = now.duration_since(SystemTime::UNIX_EPOCH).unwrap().as_secs();
    let datestamp = format!(
        "{:02}{:02}{:04}:{:02}:{:02}:{:02}:{:03}:{:03}",
        (tm_info % 12) + 1,
        (tm_info % 31) + 1,
        (tm_info / 31 / 12) + 1900,
        (tm_info / 3600) % 24,
        (tm_info / 60) % 60,
        tm_info % 60,
        0, // Millisecond
        0  // Microsecond
    );

    snapshot.set_datestamp(datestamp);

    snapshot
}


pub struct BrewDBClass {
    //conn: rusqlite::Connection,
    _conn: Arc<Mutex<rusqlite::Connection>>,
}

impl BrewDBClass {
    pub fn new() -> Result<Self> {
        let conn = Connection::open("/tmp/mydb.sqlite")?;
        Ok(BrewDBClass { _conn: Arc::new(Mutex::new(conn)), })
    }
    pub fn clone(&self) -> Self {
        Self {
            _conn: self._conn.clone(),
        }
    }
    pub fn initialize(&self) -> Result<()> {
        let conn = self._conn.lock().unwrap();
        conn.execute(
            "CREATE TABLE IF NOT EXISTS SensorState (
                iteration INTEGER,
                datestamp TEXT,
                time_since_brew_start INTEGER,
                time_since_system_start INTEGER,
                brew_switch_state BOOLEAN,
                steam_switch_state BOOLEAN,
                hot_water_switch_state BOOLEAN,
                is_steam_forgotten_on BOOLEAN,
                scales_present BOOLEAN,
                tare_pending BOOLEAN,
                temperature REAL,
                water_temperature REAL,
                pressure REAL,
                pressure_change_speed REAL,
                pump_flow REAL,
                pump_flow_change_speed REAL,
                water_pumped REAL,
                weight_flow REAL,
                weight REAL,
                shot_weight REAL,
                smoothed_pressure REAL,
                smoothed_pump_flow REAL,
                smoothed_weight_flow REAL,
                considered_flow REAL,
                pump_clicks INTEGER,
                water_lvl INTEGER,
                tof_ready BOOLEAN
            )",
            params![],
        )?;
        use std::time::Duration;
        conn.execute(
            "CREATE TABLE IF NOT EXISTS SensorStateSnapshot (
                iteration INTEGER,
                datestamp TEXT,
                time_since_brew_start INTEGER,
                time_since_system_start INTEGER,
                brew_active BOOLEAN,
                steam_active BOOLEAN,
                scales_present BOOLEAN,
                temperature REAL,
                pressure REAL,
                pump_flow REAL,
                weight_flow REAL,
                weight REAL,
                water_lvl INTEGER
            )",
            params![],
        )?;

        Ok(())
    }

    pub fn insert_sensor_state_data(&self, data: &SensorState) -> Result<()> {
        let conn = self._conn.lock().unwrap();
        conn.execute(
            "INSERT INTO SensorState (
                iteration,
                datestamp,
                time_since_brew_start,
                time_since_system_start,
                brew_switch_state,
                steam_switch_state,
                hot_water_switch_state,
                is_steam_forgotten_on,
                scales_present,
                tare_pending,
                temperature,
                water_temperature,
                pressure,
                pressure_change_speed,
                pump_flow,
                pump_flow_change_speed,
                water_pumped,
                weight_flow,
                weight,
                shot_weight,
                smoothed_pressure,
                smoothed_pump_flow,
                smoothed_weight_flow,
                considered_flow,
                pump_clicks,
                water_lvl,
                tof_ready
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            params![
                data.iteration,
                &data.datestamp,
                data.time_since_brew_start,
                data.time_since_system_start,
                data.brew_switch_state,
                data.steam_switch_state,
                data.hot_water_switch_state,
                data.is_steam_forgotten_on,
                data.scales_present,
                data.tare_pending,
                data.temperature,
                data.water_temperature,
                data.pressure,
                data.pressure_change_speed,
                data.pump_flow,
                data.pump_flow_change_speed,
                data.water_pumped,
                data.weight_flow,
                data.weight,
                data.shot_weight,
                data.smoothed_pressure,
                data.smoothed_pump_flow,
                data.smoothed_weight_flow,
                data.considered_flow,
                data.pump_clicks,
                data.water_lvl,
                data.tof_ready,
            ],
        )?;

        Ok(())
    }

    pub fn insert_sensor_state_snapshot_data(&self, data: &SensorStateSnapshot) -> Result<()> {
        let conn = self._conn.lock().unwrap();
        conn.execute(
            "INSERT INTO SensorStateSnapshot (
                iteration,
                datestamp,
                time_since_brew_start,
                time_since_system_start,
                brew_active,
                steam_active,
                scales_present,
                temperature,
                pressure,
                pump_flow,
                weight_flow,
                weight,
                water_lvl
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            params![
                data.iteration,
                &data.datestamp,
                data.time_since_brew_start,
                data.time_since_system_start,
                data.brew_active,
                data.steam_active,
                data.scales_present,
                data.temperature,
                data.pressure,
                data.pump_flow,
                data.weight_flow,
                data.weight,
                data.water_lvl,
            ],
        )?;

        Ok(())
    }

    pub fn retrieve_last_sensor_state_data_as_string(&self) -> Result<String> {
        let conn = self._conn.lock().unwrap();
        let mut stmt = conn.prepare(
            "SELECT * FROM SensorState ORDER BY iteration DESC LIMIT 1",
        )?;
        let sensor_state = stmt
            .query_row([], |row| {
                let iteration = row.get::<usize, u32>(0)?;
                let datestamp = row.get::<usize, String>(1)?;
                let time_since_brew_start = row.get::<usize, u32>(2)?;
                let time_since_system_start = row.get::<usize, u32>(3)?;
                let brew_switch_state = row.get::<usize, bool>(4)?;
                let steam_switch_state = row.get::<usize, bool>(5)?;
                let hot_water_switch_state = row.get::<usize, bool>(6)?;
                let is_steam_forgotten_on = row.get::<usize, bool>(7)?;
                let scales_present = row.get::<usize, bool>(8)?;
                let tare_pending = row.get::<usize, bool>(9)?;
                let temperature = row.get::<usize, f32>(10)?;
                let water_temperature = row.get::<usize, f32>(11)?;
                let pressure = row.get::<usize, f32>(12)?;
                let pressure_change_speed = row.get::<usize, f32>(13)?;
                let pump_flow = row.get::<usize, f32>(14)?;
                let pump_flow_change_speed = row.get::<usize, f32>(15)?;
                let water_pumped = row.get::<usize, f32>(16)?;
                let weight_flow = row.get::<usize, f32>(17)?;
                let weight = row.get::<usize, f32>(18)?;
                let shot_weight = row.get::<usize, f32>(19)?;
                let smoothed_pressure = row.get::<usize, f32>(20)?;
                let smoothed_pump_flow = row.get::<usize, f32>(21)?;
                let smoothed_weight_flow = row.get::<usize, f32>(22)?;
                let considered_flow = row.get::<usize, f32>(23)?;
                let pump_clicks = row.get::<usize, i64>(24)?;
                let water_lvl = row.get::<usize, u16>(25)?;
                let tof_ready = row.get::<usize, bool>(26)?;

                Ok(format!(
                    "Iteration: {}\nDatestamp: {}\nTime Since Brew Start: {} seconds\nTime Since System Start: {} seconds\nBrew Switch State: {}\nSteam Switch State: {}\nHot Water Switch State: {}\nIs Steam Forgotten On: {}\nScales Present: {}\nTare Pending: {}\nTemperature: {}°C\nWater Temperature: {}°C\nPressure: {} bar\nPressure Change Speed: {} bar/s\nPump Flow: {} ml/s\nPump Flow Change Speed: {} ml/s^2\nWater Pumped: {} ml\nWeight Flow: {} g/s\nWeight: {} g\nShot Weight: {} g\nSmoothed Pressure: {} bar\nSmoothed Pump Flow: {} ml/s\nSmoothed Weight Flow: {} g/s\nConsidered Flow: {} ml/s\nPump Clicks: {}\nWater Level: {}%\nTOF Ready: {}",
                    iteration,
                    datestamp,
                    time_since_brew_start,
                    time_since_system_start,
                    brew_switch_state,
                    steam_switch_state,
                    hot_water_switch_state,
                    is_steam_forgotten_on,
                    scales_present,
                    tare_pending,
                    temperature,
                    water_temperature,
                    pressure,
                    pressure_change_speed,
                    pump_flow,
                    pump_flow_change_speed,
                    water_pumped,
                    weight_flow,
                    weight,
                    shot_weight,
                    smoothed_pressure,
                    smoothed_pump_flow,
                    smoothed_weight_flow,
                    considered_flow,
                    pump_clicks,
                    water_lvl,
                    tof_ready
                ))
            })?;
        Ok(sensor_state)
    }


    fn simulator_thread_logic(db: &BrewDBClass) {
        let mut start_time = SystemTime::now();
        let mut iteration = 0;
    
        loop {
            let sensor_state = generate_fake_sensor_state(iteration);
            db.insert_sensor_state_data(&sensor_state)
                .expect("Failed to insert sensor state data");
    
            let sensor_state_snapshot = generate_fake_sensor_state_snapshot(iteration);
            db.insert_sensor_state_snapshot_data(&sensor_state_snapshot)
                .expect("Failed to insert sensor state snapshot data");
    
            iteration += 1;
    
            let elapsed_time = start_time.elapsed().unwrap();
            if elapsed_time < Duration::from_secs(5) {
                let remaining_sleep_time = Duration::from_secs(5) - elapsed_time;
                thread::sleep(remaining_sleep_time);
            } else {
                eprintln!("Warning: Loop iteration took longer than 5 seconds");
            }
    
            start_time = SystemTime::now();
        }
    }
    // fn simulator_thread_logic(){//db: Arc<Mutex<BrewDBClass>>) {
    //     let mut start_time = SystemTime::now(); // Record the start time
    //     let mut iteration = 0; // Initialize the iteration counter
    
    //     loop {
    //         let sensor_state = generate_fake_sensor_state(iteration);
    //         //let conn = self._conn.lock().unwrap();                
    //             self.insert_sensor_state_data(&sensor_state)
    //             .expect("Failed to insert sensor state data");
    
    //         let sensor_state_snapshot = generate_fake_sensor_state_snapshot(iteration);
    //         //db.lock()
    //             //.expect("Failed to acquire lock")
    //             self:insert_sensor_state_snapshot_data(&sensor_state_snapshot)
    //             .expect("Failed to insert sensor state snapshot data");
    
    //         iteration += 1; // Increment the iteration counter
    
    //         let elapsed_time = start_time.elapsed().unwrap(); // Calculate elapsed time
    //         if elapsed_time < Duration::from_secs(5) {
    //             // Calculate the remaining sleep time to achieve a 5-second interval
    //             let remaining_sleep_time = Duration::from_secs(5) - elapsed_time;
    //             thread::sleep(remaining_sleep_time);
    //         } else {
    //             // If the loop took longer than 5 seconds, log a warning
    //             eprintln!("Warning: Loop iteration took longer than 5 seconds");
    //         }
    //         // Reset the start time for the next iteration
    //         start_time = SystemTime::now();
    //     }
    // }

    // pub fn start_simulator_thread(&self) {
    //     let db_clone = self.clone();
    //     thread::spawn(move || Self::simulator_thread_logic(db_clone));
    // }
    // pub fn start_simulator_thread(&self) {
    //     //let db_clone = self._conn.clone();
    //     thread::spawn(move || Self::simulator_thread_logic());
    // }
    // pub fn start_simulator_thread(&self) {
    //     let db = self.clone(); // Clone the instance
    //     thread::spawn(move || self::simulator_thread_logic(&db, &self));
    // }
    pub fn start_simulator_thread(&self) {
        let db = self.clone(); // Clone the instance
        thread::spawn(move || BrewDBClass::simulator_thread_logic(&db));
    }
    
}

