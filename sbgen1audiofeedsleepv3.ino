#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_USE_GPRS true
#define DEBUG

#include <EEPROM.h>
#include <TinyGsmClient.h>
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#include <RTClib.h>
#include "LowPower.h"

TMRpcm audio;
RTC_DS3231 rtc;
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

String deviceId = "SEMA2";
const char apn[] = "internet";
const char server[] = "sunbird-sema-django-env.eba-yxmbx6t2.eu-west-1.elasticbeanstalk.com";
const char resource[] = "/audio/";
const int portx = 80;
byte buff[1024];
bool x;

int audiofile = 0;
unsigned long i = 0;
bool recmode = 0;
String recName = "";
int recFreq = 16000;
volatile bool recStarted = false;
volatile bool interrecord = false;
volatile bool interupload = false;
bool recStartFlag = false;

int button_presses = 1;
unsigned long recStartTime = 0UL;
unsigned long recDuration = 8000UL; 

int gsmPin = 10;
int recPin = A0;
int ledPin = 13;
int btnPin = 18;
int CLOCK_INTERRUPT_PIN = 2;
#define SD_ChipSelectPin 53

void (*resetFunc)(void) = 0;

void setup() {

  SerialMon.begin(115200);
  delay(1000);

  #ifdef DEBUG
  SerialMon.println("Loading Operating System");
  #endif

  if(!rtc.begin()) {
      SerialMon.println("Couldn't find RTC!");
      SerialMon.flush();
      while (1) delay(10);
  }

  if(rtc.lostPower()) {
      // this will adjust to the date and time at compilation
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // enforce time adjustment on firmware reupload
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  rtc.disable32K();
  
  #ifdef DEBUG
  SerialMon.println("Time Tracking RTC Activated");
  #endif

  delay(200);

  pinMode(recPin, INPUT);          
  pinMode(gsmPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP); 
  
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disableAlarm(2);

  digitalWrite(ledPin, LOW);
  digitalWrite(gsmPin, LOW);

  /*
   * Uncomment the line below to reset eeprom value manually
   * This is when debugging
  */
//   EEPROM.update(0, 1);
   
  /*
   * Read last button press from eeprom address 0
   * use it to guide on the next audio file to record
  */ 
  button_presses = EEPROM.read(0);

  if(button_presses > 100){
      //reset eeprom  only uncomment when wrong value in eeprom
      EEPROM.update(0, 1);
      button_presses == 1;
  }
   /*
     * reset eeprom and btn presses to 1 if the value is not a number
     * this happens due to sd card corruption
   */
//  if( !isDigit(button_presses)){
//      EEPROM.update(0, 1);
//      button_presses == 1;
//  }

  else
  {
    //otherwise read the eeprom value and continue recording
    recName = String(button_presses)+".wav";
  }
  
  #ifdef DEBUG
  SerialMon.print("eeprom value: ");
  SerialMon.println(button_presses);
  SerialMon.print("Next File: ");
  SerialMon.println(recName);
  #endif

  bool opencard = SD.begin(SD_ChipSelectPin);
  delay(5000);
  
//  if (SD.begin(SD_ChipSelectPin)) {
//    #ifdef DEBUG
//    SerialMon.println("SD card Success");
//    #endif
//  }
//  else {
//    #ifdef DEBUG
//    SerialMon.println("SD card Failure");
//    #endif
//  }

  if(opencard == true){
    #ifdef DEBUG
    SerialMon.println("SD card Success");
    #endif
  }
  else{
    #ifdef DEBUG
    SerialMon.println("SD card Failed");
    #endif    
  }
  
  audio.CSPin = SD_ChipSelectPin;

  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  
  //enableCommunication();
  
  #ifdef DEBUG
  SerialMon.println("System Boot Sequence Complete");
  #endif

  #ifdef DEBUG
  for (int r=1; r<6; r++){
    SerialMon.print("*");
    delay(100);
  }
  #endif
  
  #ifdef DEBUG
  SerialMon.println("");
  SerialMon.println("Voice Feedback Activated");
  SerialMon.println("");
  SerialMon.print("Device ID: ");
  SerialMon.println(deviceId);
  #endif

 if(!rtc.setAlarm1(
          DateTime(0, 0, 0, 21, 0, 0),
          DS3231_A1_Hour
  )) {
      SerialMon.println("Error, alarm wasn't set!");
  }else {
      SerialMon.println("");
      SerialMon.println("Alarm is Set!");
  }
  
}

void loop() {

  /*
   * Set Upload times
  */
  //rtc.setAlarm1(DateTime(2023, 3, 9, 21, 0, 0), DS3231_A1_Day);
  //rtc.setAlarm1(DateTime(0, 0, 0, 18, 34, 0), DS3231_A1_Hour);
  //rtc.setAlarm1(rtc.now() + TimeSpan(0, 0, 0, 10), DS3231_A1_Second); 

//  if(!rtc.setAlarm1(
//          DateTime(0, 0, 0, 14, 57, 0),
//          DS3231_A1_Hour
//  )) {
//      SerialMon.println("Error, alarm wasn't set!");
//  }else {
//      SerialMon.println("");
//      SerialMon.println("Alarm is Set!");
//  }

  attachInterrupt(digitalPinToInterrupt(btnPin), wakeUp, FALLING);

  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, LOW);

  #ifdef DEBUG
  SerialMon.println("Going to Sleep!");
  #endif

  delay(200);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  #ifdef DEBUG
  SerialMon.println("Device woke up!");
  #endif

  delay(100);
  
  if(interrecord == true)
  {
    detachInterrupt(digitalPinToInterrupt(btnPin));
    interupload == false;
    #ifdef DEBUG
    SerialMon.println("woke due to button!");  
    #endif
    startRec();
    interrecord = false;
  }
 
  if(interupload == true)
  {
    interrecord == false;
    #ifdef DEBUG
    SerialMon.println("woke due to rtc!");  
    #endif
    

    #ifdef DEBUG
    SerialMon.print("POST SQW : ");
    SerialMon.println(digitalRead(CLOCK_INTERRUPT_PIN));
    #endif
    
    if (rtc.alarmFired(1)) {
      rtc.clearAlarm(1);
      #ifdef DEBUG
      SerialMon.println("Alarm cleared!");
      #endif
    }

    checkTime();
    
    interupload = false;
  }

  #ifdef DEBUG
  for(int i=0; i<6; i++){
    SerialMon.print("*");
    delay(100);
  }
  #endif
  detachInterrupt(digitalPinToInterrupt(btnPin));

}

/*
 * Function to Restart Unit to recover if Frozen 
*/
void restartDevice()
{
  #ifdef DEBUG
  SerialMon.println("Restarting Device --- Timer Elapsed");
  #endif
  resetFunc();
}

/*
 * Function to Record Sound
 * Adjust delay value to +/- recording duration
*/
void startRec()
{
  digitalWrite(ledPin, HIGH);
  audio.startRecording(recName.c_str(), recFreq, recPin );
  #ifdef DEBUG
  SerialMon.println("recording Started");
  #endif
  delay(15000);
  
  audio.stopRecording( recName.c_str()) ;
  #ifdef DEBUG
  SerialMon.println("recording Stopped");
  #endif
  digitalWrite(ledPin, LOW);

  button_presses++;
  EEPROM.update(0, button_presses);
  recName = String(button_presses)+".wav";
  #ifdef DEBUG
  SerialMon.println("next File: "+recName);
  #endif
  
}

/*
 * Interrupt handler for button
*/
void wakeUp()
{
    // Just a handler for the pin interrupt.
    #ifdef DEBUG
    SerialMon.println("Button Pressed!");
    #endif
    interrecord = true;
}

/*
 * Interrupt handler for Alarm
*/
void onAlarm() 
{
    detachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN));
    #ifdef DEBUG
    SerialMon.println("Alarm Occured!");
    #endif
    interupload = true;
    #ifdef DEBUG
    SerialMon.print("PRE SQW: ");
    SerialMon.println(digitalRead(CLOCK_INTERRUPT_PIN));
    #endif
}

/*
 * Function to Upload Files with RTC Alarm Trigger
*/
void checkTime()
{

      #ifdef DEBUG
      SerialMon.println("---Uploading Started--");
      #endif
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);

      enableCommunication();

      bool opennow = SD.begin(SD_ChipSelectPin);
      delay(2000);
      
      if(opennow == true){
        #ifdef DEBUG
        SerialMon.println("SD card Success");
        #endif
      }
      else{
        #ifdef DEBUG
        SerialMon.println("SD card Failed");
        #endif    
      }
     
      for (int i=1; i<button_presses; i++){

        #ifdef DEBUG
        SerialMon.print("Uploading file ");  
        SerialMon.println(i);
        #endif
        
        x = uploadWavFile(i, deviceId); 
         
        if (x == true) {
          #ifdef DEBUG
          SerialMon.println("PASSED: Upload");
          #endif
          String xname = String(i)+".wav";
          
          if (SD.exists(xname)) {
            
            SD.remove(xname);
            #ifdef DEBUG
            SerialMon.println("Deleted: "+xname);
            #endif
          }
    
          delay(200);

          if(i == (button_presses - 1)){
            
            #ifdef DEBUG
            SerialMon.println("Reset Counter Now");
            #endif
            
            button_presses = 1;  //reset btn counter
            EEPROM.update(0, button_presses);
          }
        }
        else {
          #ifdef DEBUG
          SerialMon.println("FAILED: Upload");
          #endif
          EEPROM.update(0, button_presses);    
        }   
      }

      #ifdef DEBUG
      SerialMon.println("---Uploading Complete--");
      #endif

      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200); 

      disableCommunication();
}
