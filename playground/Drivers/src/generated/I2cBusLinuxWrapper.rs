/* automatically generated by rust-bindgen 0.60.1 */

pub type I2cBusLinuxWrapperHandle = *mut ::std::os::raw::c_void;
extern "C" {
    pub fn I2cBusLinuxWrapper_Create(
        busId: ::std::os::raw::c_int,
        name: *const ::std::os::raw::c_char,
    ) -> I2cBusLinuxWrapperHandle;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_Destroy(instance: I2cBusLinuxWrapperHandle);
}
extern "C" {
    pub fn I2cBusLinuxWrapper_IsValid(instance: I2cBusLinuxWrapperHandle) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_Reset(instance: I2cBusLinuxWrapperHandle);
}
extern "C" {
    pub fn I2cBusLinuxWrapper_ReadData(
        instance: I2cBusLinuxWrapperHandle,
        devAddr: ::std::os::raw::c_uchar,
        data: *mut ::std::os::raw::c_uchar,
        byteCount: ::std::os::raw::c_uint,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_ReadByte(
        instance: I2cBusLinuxWrapperHandle,
        devAddr: ::std::os::raw::c_uchar,
        data: *mut ::std::os::raw::c_uchar,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_WriteData(
        instance: I2cBusLinuxWrapperHandle,
        devAddr: ::std::os::raw::c_uchar,
        data: *mut ::std::os::raw::c_uchar,
        byteCount: ::std::os::raw::c_uint,
        timeoutMsec: ::std::os::raw::c_uint,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_WriteByte(
        instance: I2cBusLinuxWrapperHandle,
        devAddr: ::std::os::raw::c_uchar,
        data: ::std::os::raw::c_uchar,
    ) -> ::std::os::raw::c_int;
}
extern "C" {
    pub fn I2cBusLinuxWrapper_WriteReadData(
        instance: I2cBusLinuxWrapperHandle,
        devAddr: ::std::os::raw::c_uchar,
        command: *mut ::std::os::raw::c_uchar,
        commandByteCount: ::std::os::raw::c_uint,
        data: *mut ::std::os::raw::c_uchar,
        dataByteCount: ::std::os::raw::c_uint,
        timeoutMsec: ::std::os::raw::c_uint,
    ) -> ::std::os::raw::c_int;
}
