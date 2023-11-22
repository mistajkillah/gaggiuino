// Include the I2cBusTrait definition if it's in a separate file or module.
mod i2c_bus_trait {
    pub trait I2cBusTrait {
        fn reset(&self) -> GenericDriverStatus;
        fn read_data(&self, dev_addr: u8, data: &mut [u8]) -> GenericDriverStatus;
        fn read_byte(&self, dev_addr: u8, data: &mut u8) -> GenericDriverStatus;
        fn write_data(
            &self,
            dev_addr: u8,
            data: &[u8],
            byte_count: u32,
            timeout_msec: u32,
        ) -> GenericDriverStatus;
        fn write_byte(&self, dev_addr: u8, data: u8) -> GenericDriverStatus;
        fn write_read_data(
            &self,
            dev_addr: u8,
            command: &[u8],
            command_byte_count: u32,
            data: &mut [u8],
            data_byte_count: u32,
            timeout_msec: u32,
        ) -> GenericDriverStatus;
        fn set_bus_speed(&self, full_speed: bool);
        fn get_name(&self) -> &str;
        fn empty() -> Self;
    }
}
