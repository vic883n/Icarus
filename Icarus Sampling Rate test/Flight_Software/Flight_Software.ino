/* Version 1 of this code simply creates a new file every loop iteration and prints a simple message in the CSV file.
 * Version 2 will print the message 1000 times, and then create a new file.
 * Version 3 will take a time stamp from the RTC and use that to create a filename 'HHMMSS'.csv
 * Version 4 will add a green LED light that indicates the SD card has been initialized successfully
 * Version 5 will print a header followed by magnetometer x y and z values to a file
 * 
 * LAST WORKING VERSION: Version 5
*/

#include <Wire.h>
#include <SPI.h>
#include "SdFat.h"
#include <Adafruit_GPS.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3MDL.h>


#define GPSSerial Serial1
#define SD_LED 7
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h

/****************************SD CARD SETUP**************************************/

//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

//Test file
SdFile file;

//RTC object for time stamp
RTC_DS1307 rtc;

//LIS3MDL magnetometer object
Adafruit_LIS3MDL lis3mdl;
int magX, magY, magZ;

char charFileName [20];
int dataCount = 0;
String fileName = " ";

/********************************GPS SETUP**********************************/
boolean usingInterrupt = false;
void useInterrupt(boolean);
Adafruit_GPS GPS (&GPSSerial);

void setup() 
{
  pinMode(SD_LED, OUTPUT);
  digitalWrite(SD_LED, HIGH);
  
  rtc.begin();
  
  Serial.begin(9600);
  if (!sd.begin(SD_CHIP_SELECT_PIN))
  {
    sd.initErrorHalt();
    digitalWrite(SD_LED, LOW);
  }
  
  String fileName = createFileName();
  fileName.toCharArray(charFileName, 20);

  GPS.begin(9600);

  GPS.sendCommand(PMTK_Q_RELEASE);
  GPS.sendCommand(PGCMD_NOANTENNA);
  //GPS.sendCommand(PGCMD_ANTENNA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);

  useInterrupt(true);

/*****************************************MAGNETOMETER SETUP*****************************/  
  Serial.println("Adafruit LIS3MDL test!");
  if (! lis3mdl.begin_I2C())
  {
    Serial.println("Failed to find LIS3MDL chip");
    while (1)
    {
      delay(10);                                                        //ADD CODE FOR MAG LED HERE!!!
    }
    Serial.println("LIS3MDL Found!");
  }
  lis3mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);                       //Set mag performance mode here
  Serial.print("Performance mode set to: ");                            //Print selected performance mode
  switch (lis3mdl.getPerformanceMode()) 
  {
    case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
    case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
    case LIS3MDL_HIGHMODE: Serial.println("High"); break;
    case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
  }
  lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);                     //Set output mode. Maybe revisit this to output only when asked for?
  Serial.print("Operation mode set to: ");
  // Single shot mode will complete conversion and go into power down
  switch (lis3mdl.getOperationMode()) 
  {
    case LIS3MDL_CONTINUOUSMODE: Serial.println("Continuous"); break;
    case LIS3MDL_SINGLEMODE: Serial.println("Single mode"); break;
    case LIS3MDL_POWERDOWNMODE: Serial.println("Power-down"); break;
  }

  
  
}

SIGNAL(TIMER0_COMPA_vect)
{
  char c = GPS.read();
}

void useInterrupt(boolean v)
{
  if (v)
  {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  }

  else
  {
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}


String createFileName()
{
  DateTime time =rtc.now();
  String temp = " ";
  String currentTime = " ";
    temp = String(time.hour());
    if (time.hour() < 10)
    {
      temp = "0"+String(time.hour(),DEC);
    }
    currentTime = temp;

    temp = String(time.minute());
    if (time.minute() < 10)
    {
     temp = "0"+String(time.minute(),DEC);
    }
    currentTime += temp;

    temp = String(time.second());
   if (time.second() < 10)
   {
     temp = "0"+String(time.second(),DEC);
   }
  currentTime += temp;
  currentTime += ".csv";
  return currentTime;
}

void getMagValues()
{
  lis3mdl.read();
  magX = lis3mdl.x;
  magY = lis3mdl.y;
  magZ = lis3mdl.z;
}

void loop() 
{
  #else  // ENABLE_SOFTWARE_SPI_CLASS
  #error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
  #endif 
  
  if (dataCount == 1000)
  {  
    fileName = createFileName();
    fileName.toCharArray(charFileName, 20);
    dataCount = 0;
  }
  
  if (!file.open(charFileName, O_RDWR | O_CREAT | O_AT_END))
  {
    sd.initErrorHalt(F("open failed"));
  }
  
  if (dataCount == 0)
  {
    file.println(F("mag x, mag y, mag z"));
    file.close();
  }
  
  getMagValues();
 
  String magPrintX (magX, DEC);
  String magPrintY (magY, DEC);
  String magPrintZ (magZ, DEC);

  file.open(charFileName, O_AT_END);
  file.println(magPrintX + "," + magPrintY + "," + magPrintZ);
  file.close();
  
  if (dataCount < 1000)
  {
    Serial.println (dataCount);
  }
  else
  {
    Serial.println("Done");
  }
  dataCount ++;
  Serial.println(dataCount);  
}
