// // extern crate chrono;
//  use std::thread; // Import the thread module
//  mod BrewDBClass;

// // use BrewDBClass::BrewDBClass;

// extern crate chrono;
// use std::time::Duration;
// use std::time::SystemTime;
// use chrono::Local;
// //use std::time::SystemTime; // Add this line to import SystemTime

// use BrewDBClass::BrewDBClass;
// use BrewDBClass::SensorState;
// use BrewDBClass::SensorStateSnapshot;
// use rusqlite::Result; // Import Result type from rusqlite
#[allow(non_snake_case)]
use std::process::Command;
extern crate chrono;
use std::thread;
use rand::Rng;
use std::time::Duration;
use std::time::SystemTime;
mod BrewDBLib;

use BrewDBLib::BrewDBClass;
//use BrewDBLib::SensorState;
//use BrewDBLib::SensorStateSnapshot;
//use rusqlite::Result;


fn main() {
    let _rmDB = Command::new("rm")
    .arg("-rf")
    .arg("/tmp/mydb.sqlite")
    .output();
    let db = BrewDBClass::new().expect("Failed to open database");
    let db_clone = db.clone(); // Clone the Arc to move ownership into the closure
    // Initialize the database and tables
    db.initialize().expect("Failed to initialize database");

    db.start_simulator_thread();
    // // Start the simulator thread
    // // Start the simulator thread
    // let _simulator_thread = thread::spawn(move || {
    // let mut start_time = SystemTime::now(); // Record the start time
    // let mut iteration = 0; // Initialize the iteration counter
    // loop {

    //     let sensor_state = generate_fake_sensor_state(iteration);
    //     db_clone.insert_sensor_state_data(&sensor_state)
    //         .expect("Failed to insert sensor state data");

    //     let sensor_state_snapshot = generate_fake_sensor_state_snapshot(iteration);
    //     db_clone.insert_sensor_state_snapshot_data(&sensor_state_snapshot)
    //         .expect("Failed to insert sensor state snapshot data");

    //     iteration += 1; // Increment the iteration counter

    //     let elapsed_time = start_time.elapsed().unwrap(); // Calculate elapsed time
    //     if elapsed_time < Duration::from_secs(5) {
    //         // Calculate the remaining sleep time to achieve a 5-second interval
    //         let remaining_sleep_time = Duration::from_secs(5) - elapsed_time;
    //         thread::sleep(remaining_sleep_time);
    //     } else {
    //         // If the loop took longer than 5 seconds, log a warning
    //         eprintln!("Warning: Loop iteration took longer than 5 seconds");
    //     }
    //     // Reset the start time for the next iteration
    //     start_time = SystemTime::now();
    //     }
    // });


    let mut start_time = SystemTime::now(); // Record the start time

    loop {

        // Add any additional functionality here
        thread::sleep(Duration::from_secs(1));

        let last_data = db
            .retrieve_last_sensor_state_data_as_string()
            .expect("Failed to retrieve sensor state data");
        println!("{}", last_data);

        let elapsed_time = start_time.elapsed().unwrap(); // Calculate elapsed time
        if elapsed_time < Duration::from_secs(5) {
            // Calculate the remaining sleep time to achieve a 5-second interval
            let remaining_sleep_time = Duration::from_secs(5) - elapsed_time;
            thread::sleep(remaining_sleep_time);
        } else {
            // If the loop took longer than 5 seconds, log a warning
            eprintln!("Warning: Loop iteration took longer than 5 seconds");
        }
                // Reset the start time for the next iteration
                start_time = SystemTime::now();
    }

    // Wait for the simulator thread to finish (this won't happen in this code)
   // simulator_thread.join().unwrap();
}
