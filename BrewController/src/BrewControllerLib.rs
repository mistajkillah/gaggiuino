//use SimpleKalmanFilter;
//mod SimpleKalmanFilterLib;
//use SimpleKalmanFilter;
// Import the SimpleKalmanFilter module
extern crate rust_pigpio;
use ::function_name::named;
use crate::BrewDBLib::BrewDBClass;
use crate::SimpleKalmanFilterLib::SimpleKalmanFilter;
use crate::LoggerLib::Logger;
use std::sync::{Arc, Mutex, Once, RwLock};
pub struct BrewController {
    _smooth_pressure: Option<SimpleKalmanFilter>,
    _smooth_pump_flow: Option<SimpleKalmanFilter>,
    _smooth_scales_flow: Option<SimpleKalmanFilter>,
    _smooth_considered_flow: Option<SimpleKalmanFilter>,
    _myBrewDB: Option<BrewDBClass>,
    _mySensorState:Option<SensorState>,
    _zCPin: u32,
    _brewPin: u32, 
    _boilerRelayPin:u32,
    _dimmerPin:u32,
    _steamPin:u32,
    _valvePin:u32,
    _waterPin:u32,
}


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
    pub boilerState:bool
}
impl SensorState {
    pub fn new() -> Self {
        Self {
            iteration: 0,
            datestamp: String::new(),
            time_since_brew_start: 0,
            time_since_system_start: 0,
            brew_switch_state: false,
            steam_switch_state: false,
            hot_water_switch_state: false,
            is_steam_forgotten_on: false,
            scales_present: false,
            tare_pending: false,
            temperature: 0.0,
            water_temperature: 0.0,
            pressure: 0.0,
            pressure_change_speed: 0.0,
            pump_flow: 0.0,
            pump_flow_change_speed: 0.0,
            water_pumped: 0.0,
            weight_flow: 0.0,
            weight: 0.0,
            shot_weight: 0.0,
            smoothed_pressure: 0.0,
            smoothed_pump_flow: 0.0,
            smoothed_weight_flow: 0.0,
            considered_flow: 0.0,
            pump_clicks: 0,
            water_lvl: 0,
            tof_ready: false,
            boilerState: false
        }
    }
}
impl BrewController {
    fn new() -> Self {
        Self {
            _smooth_pressure: None,
            _smooth_pump_flow: None,
            _smooth_scales_flow: None,
            _smooth_considered_flow: None,
            _myBrewDB: None,
            _mySensorState: None,
            _zCPin:1,
             _brewPin:2,
             _boilerRelayPin:3,
             _dimmerPin:4,
             _steamPin:5,
             _valvePin:6,
             _waterPin:7,
        }
    }


    pub fn get_instance() -> Arc<RwLock<Self>> {
        static mut INSTANCE: Option<Arc<RwLock<BrewController>>> = None;
        static INIT: Once = Once::new();

        INIT.call_once(|| {
            let singleton = BrewController::new();
            let singleton_rwlock = Arc::new(RwLock::new(singleton));
            unsafe {
                INSTANCE = Some(singleton_rwlock);
            }
        });

        unsafe {
            INSTANCE.as_ref().unwrap().clone()
        }
    }
    pub fn SetBoilerState(&mut self, state: bool) {
        // Update the state of the boiler            
        let result:rust_pigpio::GpioResult = rust_pigpio::write(self._boilerRelayPin, if state==true {  rust_pigpio::ON} else {rust_pigpio::OFF});
        
        if let Some(sensor_state) = &mut self._mySensorState {
            sensor_state.boilerState = state;
        }


    }
    fn log(message: &str)
    {
        Logger::get_instance().log_info( message);
    }
    pub fn Initialize(&mut self)
    {
        log(function_name!());
        self._smooth_pressure = Some(SimpleKalmanFilter::new(0.6, 0.6, 0.1));
        self._smooth_pump_flow = Some(SimpleKalmanFilter::new(0.1, 0.1, 0.01));
        self._smooth_scales_flow = Some(SimpleKalmanFilter::new(0.5, 0.5, 0.01));
        self._smooth_considered_flow = Some(SimpleKalmanFilter::new(0.1, 0.1, 0.1));
        self._myBrewDB = Some(BrewDBClass::new().expect("Failed to open database"));
        self._mySensorState = Some(SensorState::new());
        let response: Result<u32, String>=rust_pigpio::initialize();

        rust_pigpio::set_mode(self._brewPin, rust_pigpio::OUTPUT);

    }
}
