#pragma once



#define SIM true
#define SIMULATION \
    if (SIM)       \
    {              \
        return 0;  \
    }




// STM32F4 pins definitions
#define thermoDO      0 //PB4
#define thermoDI      1 //PA7 // not used
#define thermoCS      2 //PA6
#define thermoCLK     3 //PA5

#define zcPin         4 //PA0
#define brewPin       5 //PC14
#define relayPin      6 //PA15
#define dimmerPin     7 //PA1
#define steamPin      8 //PC15
#define valvePin      9 //PC13
#if defined(SINGLE_BOARD)
#define waterPin      10 //PB15
#else
#define waterPin      11 //PA12
#endif

#ifdef PCBV2
// PCB V2
#define steamValveRelayPin PB12
#define steamBoilerRelayPin PB13
#endif

#define HX711_sck_1   12 //PB0
#define HX711_dout_1  13 //PB8
#define HX711_dout_2  14 //PB9

#define USART_LCD     15 //Serial2 // PA2(TX) & PA3(RX)
#define USART_ESP     16 //Serial1 // PA9(TX) & PA10(RX)
#define USART_DEBUG   17 //Serial  // USB-CDC (Takes PA8,PA9,PA10,PA11)
#define PIN_WIRE_SDA 1
#define PIN_WIRE_SCL 1

#if defined SINGLE_BOARD
    #define GET_KTYPE_READ_EVERY    70 // max31855 amp module data read interval not recommended to be changed to lower than 70 (ms)
#else
    #define GET_KTYPE_READ_EVERY    250 // max6675 amp module data read interval not recommended to be changed to lower than 250 (ms)
#endif
#define GET_PRESSURE_READ_EVERY 10 // Pressure refresh interval (ms)
#define GET_SCALES_READ_EVERY   100 // Scales refresh interval (ms)
#define REFRESH_SCREEN_EVERY    150 // Screen refresh interval (ms)
#define REFRESH_FLOW_EVERY      50 // Flow refresh interval (ms)
#define HEALTHCHECK_EVERY       30000 // System checks happen every 30sec
#define BOILER_FILL_START_TIME  3000UL // Boiler fill start time - 3 sec since system init.
#define BOILER_FILL_TIMEOUT     8000UL // Boiler fill timeout - 8sec since system init.
#define BOILER_FILL_SKIP_TEMP   85.f // Boiler fill skip temperature threshold
#define SYS_PRESSURE_IDLE       0.7f // System pressure threshold at idle
#define MIN_WATER_LVL           10u // Min allowable tank water lvl



