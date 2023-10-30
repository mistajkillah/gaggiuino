// this library is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#ifndef ADAFRUIT_MAX6675_H
#define ADAFRUIT_MAX6675_H

#include "Arduino.h"

/**************************************************************************/
/*!
    @brief  Class for communicating with thermocouple sensor
*/
/**************************************************************************/
class SPIClass {       // The class
  public:             // Access specifier
    SPIClass(int a, int b , int c)
    {
      
    }
    int myNum;        // Attribute (int variable)
    
};
class MAX6675 {
public:
  MAX6675( int8_t CS, SPICLass & spiCLass);

  float readCelsius(void);
  float readFahrenheit(void);

  /*!    @brief  For compatibility with older versions
         @returns Temperature in F or NAN on failure! */
  float readFarenheit(void) { return readFahrenheit(); }

private:
  int8_t sclk, miso, cs;
  uint8_t spiread(void);
};

#endif
