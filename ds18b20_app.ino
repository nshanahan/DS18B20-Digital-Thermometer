/***************************************************************
 * @file ds18b20_app
 * 
 * @par Nicholas Shanahan (2016)
 *
 * @brief Arduino application for DS18B20 temperature sensor.
 * Prints temperature in degrees fahrentheit approximately
 * every two seconds to the Arduino serial console.
 *
 **************************************************************/
 
/**************************************************************
        Includes
***************************************************************/
#include "DS18B20.h"
#include <stdint.h>
#include <stdbool.h>

/**************************************************************
         Defines
***************************************************************/
#define DS18B20_PIN 2

/**************************************************************
         Variables
***************************************************************/
ds18b20_dev_t dev;
bool err = false;

/**************************************************************
          Function Prototypes
***************************************************************/
void setup() 
{ 
  uint8_t idx;
  
  Serial.begin(9600);
  //setup DS18B20
  err = ds18b20_init(&dev, DS18B20_PIN);
  
  if (!err)
  {
    Serial.println("DS18B20 initialization passed!");
    err = ds18b20_get_rom(&dev); 

    if (!err)
    {
      Serial.print("DS18B20 device S/N: 0x");
      //print serial number
      for(idx = 1; idx < 7; idx++)
      {
        Serial.print(dev.rom[idx], HEX);
      }
      
      Serial.println("\nRendering temperature readings...");
    }
  }
  
  else
  {
    Serial.println("DS18B20 initialization failed!");
    exit(0);
  }
}

void loop() 
{
  float fahrenheit_temp = 0;
  err = ds18b20_read_temp(&dev);
  
  if(!err)
  {
    fahrenheit_temp = (dev.temp*9)/5 + 32;
    Serial.print("Temperature(°F): ");
    Serial.println(fahrenheit_temp, 4);
  }
  
  else
  {
    Serial.println("Couldn't read DS18B20 sensor.");
  }
  
  delay(2000);
}
