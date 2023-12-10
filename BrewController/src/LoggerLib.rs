#![allow(warnings)]
use std::sync::{Mutex, Once};

// Define constants for log levels
const LOG_INFO: u32 = 0b01;
const LOG_DEBUG: u32 = 0b10;

// Define a struct for the logger
pub struct Logger {
    log_mask: Mutex<u32>,
}

impl Logger {
    // Private constructor to ensure a single instance
    fn new() -> Logger {
        Logger {
            log_mask: Mutex::new(0b11),
        }
    }

    // Get the singleton instance of the logger
    pub fn get_instance() -> &'static Logger {
        lazy_static::lazy_static! {
            static ref LOGGER: Logger = Logger::new();
        }
        &LOGGER
    }

    // Set the log mask
    pub fn set_log_mask(&self, mask: u32) {
        let mut log_mask = self.log_mask.lock().unwrap();
        *log_mask = mask;
    }

    // Log an info message to stdout if the corresponding bit is set in the log mask
    pub fn log_info(&self, message: &str) {
        let log_mask = self.log_mask.lock().unwrap();
        if *log_mask & LOG_INFO != 0 {
            println!("INFO: {}", message);
        }
    }

    // Log a debug message to stdout if the corresponding bit is set in the log mask
    pub fn log_debug(&self, message: &str) {
        let log_mask = self.log_mask.lock().unwrap();
        if *log_mask & LOG_DEBUG != 0 {
            println!("DEBUG: {}", message);
        }
    }
}