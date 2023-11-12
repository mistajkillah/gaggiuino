/* automatically generated by rust-bindgen 0.69.1 */

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct SmoothTempSensorWrapper {
    _unused: [u8; 0],
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct TempSensorWrapper {
    _unused: [u8; 0],
}
extern "C" {
    pub fn SmoothTempSensor_Create(
        origin: *mut TempSensorWrapper,
        smoothingFactor: ::std::os::raw::c_int,
    ) -> *mut SmoothTempSensorWrapper;
}
extern "C" {
    pub fn SmoothTempSensor_Destroy(sensor: *mut SmoothTempSensorWrapper);
}
extern "C" {
    pub fn SmoothTempSensor_readCelsius(sensor: *mut SmoothTempSensorWrapper) -> f64;
}
extern "C" {
    pub fn SmoothTempSensor_readKelvin(sensor: *mut SmoothTempSensorWrapper) -> f64;
}
extern "C" {
    pub fn SmoothTempSensor_readFahrenheit(sensor: *mut SmoothTempSensorWrapper) -> f64;
}
