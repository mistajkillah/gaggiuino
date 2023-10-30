/* 09:32 15/03/2023 - change triggering comment */
#ifndef PINDEF_H
#define PINDEF_H

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

#endif
