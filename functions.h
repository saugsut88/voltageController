#include <Arduino.h> //needed for Serial.println
#include <Wire.h>
#include <string.h> //needed for memcpy
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>


//LCD setup
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

//relay setup
int mainRelay = 47;
int slaveRelayBox1 = 45;
int slaveRelayBox2 = 43;


//LED setup
int mainPowerLED = 31;
int notProtectedLED = 33;
int protectedLED = 35;



//Limit setup
double lowLimit = 0.800;
double highLimit = 0.950;

//Voltage Offset adjustment
double offset = 10;




//////////////////////////////////////////Functions/////////////////////////////////////////////////////////

//turn off relays
boolean turnoffRelays(){

  bool relayOn = false;  

  digitalWrite(mainRelay, LOW);
  digitalWrite(slaveRelayBox1, LOW);
  digitalWrite(slaveRelayBox2, LOW);
  
  return relayOn;
}

//turn on relays
boolean turnOnRelays(){

  bool relayOn = true; 

  digitalWrite(mainRelay, HIGH);
  digitalWrite(slaveRelayBox1, HIGH);
  digitalWrite(slaveRelayBox2, HIGH);
  
  return relayOn;
}


void mainPowerLED_ON(){

  digitalWrite(mainPowerLED, HIGH);
  digitalWrite(protectedLED, LOW);
  digitalWrite(notProtectedLED, LOW);

}

void systemProtectedLED(){

  digitalWrite(protectedLED, HIGH);
  digitalWrite(notProtectedLED, LOW);

}

void systemNotProtectedLED(){

  digitalWrite(protectedLED, LOW);
  digitalWrite(notProtectedLED, HIGH);

}

//setup sdcard
int sdSetup(int chipSelect){

  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH); 
  if (SD.begin(chipSelect))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    //return;
  }
  return chipSelect;
}


//read the volatege across pin A3 and ground
//returns a float valueS
double readVoltage(){

  int sensorValue = analogRead(A3);
  double volts = sensorValue * (5.0 * offset / 1023.0);
  //Serial.println(volts, 4);
  return volts;
}

//store data on SD card
void dataStorage(double volts,File myFile, DateTime now, boolean relay){
  //float voltage = volts;
  boolean relay_on = relay;
  String relayVal = "";
  double voltage = volts;
  
  if(relay_on == true){
    relayVal = "Relay on";
  }else{
    relayVal = "Relay off";
  }
  
  myFile = SD.open("voltage.txt", FILE_WRITE);  
  if (myFile) {    
    myFile.print(now.timestamp());
    myFile.print(","); 
    myFile.print(voltage, 4);
    myFile.print(",");
    myFile.println(relayVal);
    myFile.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.print("Data Not Saved");
    Serial.print(" --- ");
    Serial.println("error opening voltage.txt");
  }

}//End SD card storage


//serial monitor of data capture
void serialMonitorData(double voltage, DateTime now){
  Serial.print(voltage, 4);
  Serial.print(" --- ");
  Serial.println(now.timestamp());
  
}


/*
 * TODO
 * Screen data display functionality
 * need to add current
 */
 void screenData(double volts){

  //String relayVal = "";
  double voltage = volts;
  //double current = curr
  /*
  boolean relay_on = relay;
  if(relay_on == true){
      relayVal = "Relay on";
    }else{
      relayVal = "Relay off";
    }
  */
  String voltz = String(voltage);

  //screen print voltage, relayVal
  lcd.clear();
  lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  lcd.print("VDC: " + voltz);
  lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  lcd.print("Current: ");
  
  
 }//end screen data


 /*
  * TODO
  * Relay on or off per value of voltage potential
  */
boolean relayCheck(double volts){

  bool relayStatus;
 
  double voltage = volts;

  if(voltage <= lowLimit){
    
    //turn on relays
    relayStatus = turnOnRelays();

    //system Protected LED
    systemProtectedLED();

    Serial.println("Relay on Voltage potential to low");

  }else if(voltage >= highLimit){
  
    //relay off
    relayStatus = turnoffRelays();

    //system not protected LED
    systemNotProtectedLED();
    
    Serial.println("Relay off Voltage potential to high");
  
  }else{

    //never should reach
    Serial.println("relay/ voltage measurement issue");
  
  }

  return relayStatus;

}//end relayOn function



void LCDStart(){

  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  lcd.print("NSWCPD");
  lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  lcd.print("TSAC");

  delay(3000);

  lcd.clear();
  lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  lcd.print("Voltage");
  lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  lcd.print("controller");
  
}
