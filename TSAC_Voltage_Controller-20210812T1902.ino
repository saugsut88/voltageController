#include <SD.h>
#include <SPI.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
//user defined files to import
#include "functions.h"
#include "calibration.h"


File myFile;
const int chipSelect = 10;
boolean relayStat = false;

// define the Real Time Clock object
RTC_PCF8523 rtc; 



// the setup routine runs once when you press reset:
void setup() {
  //pinmode
  pinMode(mainRelay, OUTPUT);
  pinMode(slaveRelayBox1, OUTPUT);
  pinMode(slaveRelayBox2, OUTPUT);
  pinMode(mainPowerLED, OUTPUT);
  pinMode(notProtectedLED, OUTPUT);
  pinMode(protectedLED, OUTPUT);  


  //set Relays off on initial startup
  turnoffRelays();

  //main power LED 
  mainPowerLED_ON();

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //pinMode(pinCS, OUTPUT);

  Serial.println("set up started");
  
  //check for card
  sdSetup(chipSelect);

  //Start real time clock
  rtc.begin();

  //set up screen
  LCDStart();

  

  //set up calibration

  //use this line to adjust real time clock current date and time, spi pins missing off of the data logger board does not work at the moment
  //example 2021, august 11, 0800
  //rtc.adjust(DateTime(2021, 8, 12, 16, 44, 0));
  
}//end setup

// the loop routine runs over and over again forever:
void loop() {
  
  DateTime now = rtc.now();
  
  //calls function to read voltage(see functions header)
  double voltage = readVoltage();

  // print out the value you read, formatted to 4 decimal places:
  serialMonitorData(voltage, now);


    /*
   * TODO
   * Realy control
   */
  //relay = relayOn(voltage);
  relayStat = relayCheck(voltage);

  //open file, create if it doesnt exist
  //dataStorage(voltage, myFile, now, relay);

  /*
   * TODO
   * Screen data display, need to add CUrrent once Hall effect sensor is in
   */
  screenData(voltage);


  //repeat every 3 seconds (can change at a later date)
  delay(3000);
  
}//end main
