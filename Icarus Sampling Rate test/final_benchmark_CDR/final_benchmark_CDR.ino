/*
dataPacket_Test
Version 2
creates and fills an integer array with 13 elements, 1000 times.
This allows me to find an average time to fill all 13 elements
with their respective data types.

Version 2 writes the array to an SD card.
*/
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>


#define CS 10
#define MOSI 11
#define MISO 12
#define CLK 13
#define GPSSerial Serial1


/*******************Global filename variable****************************/
File myFile;
char fileName[21] = "lol1test.csv";

/*******************Global interrupt variable***************************/
boolean usingInterrupt = false;
void useInterrupt(boolean);

/*******************Global dataPacket array*************************/
int dataPacket[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
int counter = 0;
int c,d,p;
int uvTimer = millis();
int difference;

/*******************Global enviroPacket array***********************/
int enviroPacket[4] = {0,0,0,0};

/*******************Global NMEA sentence string variable***************/
String nmeaSentence = "$GPGGA,xxxxxx,xxxx.xx,x,xxxx.xx,x,x,xx,x.x,xxxxxx.x,x,xx.x,x,,*xx";
char newLine = 13;
int indexOfValue = 0;
int gpsTimer = millis();
Adafruit_GPS GPS (&GPSSerial);


void setup() 
{
  Serial.begin(9600);
  
  /****Setting up the SD card******************/
  Serial.println("Initializing SD card...");
  if (!SD.begin(CS,MOSI,MISO,CLK))
  {
    Serial.println("Initialization failed :(");
    while(1);
  }
  Serial.println("Initialization successful!");

  /***************Setting up UV ADC pins*********************/
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  /**************Setting up environmental ADC pins***********/
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

  /********************Setting up the GPS********************/
  GPS.begin(9600);

  GPS.sendCommand(PMTK_Q_RELEASE);
  GPS.sendCommand(PGCMD_NOANTENNA);
  //GPS.sendCommand(PGCMD_ANTENNA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);

  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GLLONLY);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_VTGONLY);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GSVONLY);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);

  useInterrupt(true);
  Wire.begin();


  enviroPacket[0] = analogRead(A4);
  enviroPacket[1] = analogRead(A5);
  enviroPacket[2] = analogRead(A6);
  enviroPacket[3] = analogRead(A7);

  
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

/*
 *This function prints the filled array to the serial
 *monitor
 */
void printArray(int dataPacket[13])
{
  for (int z = 0; z < 13; z++)
  {
    Serial.print(dataPacket[z]);
    Serial.print(", ");
  }
  Serial.println();
  //return;
}

/*
 * This function prints the filled array to an external
 * SD card
*/
void printToSD(int dataPacket[13])
{
  myFile = SD.open(fileName, FILE_WRITE);
  for (int i = 0; i < 13; i++)
  {
    myFile.print(dataPacket[i]);
    if (i < 12)
    {
      myFile.print(",");
    }
  }
  myFile.println();
  myFile.close();
  //return;
}

/********************This function fills the dataPacket array*****************/
void fillDataPacket ()
{
    dataPacket[0] = analogRead(A0);
    dataPacket[1] = analogRead(A1);
    dataPacket[2] = analogRead(A2);
    dataPacket[3] = analogRead(A3);
    for (int i = 0; i < 9; i++)
    {
      Wire.beginTransmission(2);
      Wire.requestFrom (2,2);
      while (Wire.available())
      {
        c = Wire.read();
        d = Wire.read();
        p = (c * pow(2,8) ) + d;
        dataPacket[i+4] = p;
      }
      Wire.endTransmission();
    }
      //return;
}

/*******************This function fills the enviroPacket**********************/
void fillEnviroPacket ()
{
  enviroPacket[0] = analogRead(A4);
  enviroPacket[1] = analogRead(A5);
  enviroPacket[2] = analogRead(A6);
  enviroPacket[3] = analogRead(A7);
  //return;
}

/*******************This function receives the NMEA sentence******************/
void getNMEA ()
{
  if (GPS.newNMEAreceived())
  {
    nmeaSentence = (GPS.lastNMEA());
    indexOfValue = nmeaSentence.indexOf(newLine);
    nmeaSentence.remove(indexOfValue);
  }
}

/*******************This function opens a file, writes the data entry, and closes the file*/
void writeData()
{
  myFile = SD.open(fileName, FILE_WRITE);
  for (int i = 0; i < 13; i++)
  {  
    myFile.print(dataPacket[i]);
    myFile.print(",");
  }
  myFile.print(nmeaSentence);
  myFile.print(",");
  for (int j = 0; j < 4; j++)
  {
    myFile.print(enviroPacket[j]);
    myFile.print(",");
  }
  myFile.print("\n");
  myFile.close();
  //return;
}

void loop() 
{
  Serial.println(millis());
  while(counter < 10000)
  {
    difference = millis() - uvTimer;
    if (difference >= 167)
    {
      uvTimer = millis();
      fillDataPacket(); 
    }
    else
    {
      delay(167-difference);
      uvTimer = millis();
      fillDataPacket();
    }
    if (millis() - gpsTimer >= 2000)
    {
      gpsTimer = millis();
      getNMEA();
      fillEnviroPacket();
    }
    writeData();
    counter++;
  }
  Serial.println(millis());
  while(1);
}
