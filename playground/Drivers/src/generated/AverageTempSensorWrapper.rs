/* automatically generated by rust-bindgen 0.59.1 */

extern "C" {
    pub fn AverageTempSensor_Create(
        origin: *mut ::std::os::raw::c_void,
        readingsNumber: ::std::os::raw::c_int,
        delayTimeInMillis: ::std::os::raw::c_int,
    ) -> *mut ::std::os::raw::c_void;
}
extern "C" {
    pub fn AverageTempSensor_Destroy(sensor: *mut ::std::os::raw::c_void);
}
extern "C" {
    pub fn AverageTempSensor_readCelsius(sensor: *mut ::std::os::raw::c_void) -> f64;
}
extern "C" {
    pub fn AverageTempSensor_readKelvin(sensor: *mut ::std::os::raw::c_void) -> f64;
}
extern "C" {
    pub fn AverageTempSensor_readFahrenheit(sensor: *mut ::std::os::raw::c_void) -> f64;
}
