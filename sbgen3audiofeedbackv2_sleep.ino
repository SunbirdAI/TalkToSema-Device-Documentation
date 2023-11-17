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

// String deviceId = "SEMA2";
// String deviceId = "SEMA-PILOT";
String deviceId = "TESTUNIT";
const char apn[] = "internet";
const char server[] = "sunbird-sema-django-env.eba-yxmbx6t2.eu-west-1.elasticbeanstalk.com";
// const char server[] = "noise-sensors-dashboard.herokuapp.com";
const char resource[] = "/audio/";
const int portx = 80;
byte buff[1024];
bool x;

int audiofile = 0;
unsigned long i = 0;
bool recmode = 0;
String recName = "1.wav";
String dataStringtime = "";
String rtcstamp ="";
String finaltime ="";
String supportfilename = "";
String audiofilename = "";
int recFreq = 16000;
volatile bool recStarted = false;
volatile bool interrecord = false;
volatile bool interupload = false;
bool recStartFlag = false;

int button_presses = 1;
unsigned long recStartTime = 0UL;
unsigned long recDuration = 5000UL; 

int gsmPin = 10;
int recPin = A0;
int ledPin = 13;
// int btnPin = 3; //NEW SETUP
int btnPin = 18; //OLD SETUP
int CLOCK_INTERRUPT_PIN = 2;
#define SD_ChipSelectPin 53

void (*resetFunc)(void) = 0;

DateTime now;

void setup() {

  Serial.begin(115200);
  delay(1000);

  #ifdef DEBUG
  Serial.println("Loading Operating System");
  #endif

  if(!rtc.begin()) {
      Serial.println("Couldn't find RTC!");
      Serial.flush();
      while (1) delay(10);
  }
  
  /*
  Uncomment to correct time when uploading
  */
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.disable32K();
  #ifdef DEBUG
  Serial.println("RealTimeClock Active");
  #endif
  
  delay(200);

  pinMode(recPin, INPUT);          
  pinMode(gsmPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);

  digitalWrite(ledPin, LOW);
  digitalWrite(gsmPin, LOW);

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disableAlarm(2);

  /*
   * Uncomment the line below to reset eeprom value manually
   * This is when debugging
  */
  // EEPROM.update(0, 1);

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
  else
  {
    //otherwise read the eeprom value and continue recording
    recName = String(button_presses)+".wav";
  }

  #ifdef DEBUG
  Serial.print("eeprom value: ");
  Serial.println(button_presses);
  Serial.print("Next Feedback File: ");
  Serial.println(recName);
  #endif
  
  //check if sd card is okay
  SdCardSequence();

  audio.CSPin = SD_ChipSelectPin;

  ledOffSequence();

  #ifdef DEBUG
  Serial.println("Boot Sequence Complete");
  #endif

  #ifdef DEBUG
  for (int r=1; r<6; r++){
    Serial.print("*");
    delay(100);
  }
  #endif

  #ifdef DEBUG
  Serial.println("");
  Serial.println("Voice Feedback Activated");
  Serial.println("");
  Serial.print("Device ID: ");
  Serial.println(deviceId);
  #endif

  DateTime time = rtc.now();
  String rtcstamp = time.timestamp(DateTime::TIMESTAMP_FULL);

  #ifdef DEBUG
  Serial.println(rtcstamp);
  #endif

  if(!rtc.setAlarm1(
          DateTime(0, 0, 0, 12, 19, 0),
          DS3231_A1_Hour
  )) {
      SerialMon.println("Error, alarm wasn't set!");
  }else {
      SerialMon.println("");
      SerialMon.println("Alarm is Set!");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  attachInterrupt(digitalPinToInterrupt(btnPin), wakeUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, LOW);

  #ifdef DEBUG
  SerialMon.println("Going to bed!");
  #endif

  delay(200);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  #ifdef DEBUG
  SerialMon.println("Device woke up!");
  #endif

  now = rtc.now();
  
  /*
  Uncomment when debugging to view time
  */
  // #ifdef DEBUG
  // Serial.print(now.year(), DEC);
  // Serial.print('/');
  // Serial.print(now.month(), DEC);
  // Serial.print('/');
  // Serial.print(now.day(), DEC);
  // Serial.print(" ");
  // Serial.print(now.hour(), DEC);
  // Serial.print(':');
  // Serial.print(now.minute(), DEC);
  // Serial.print(':');
  // Serial.print(now.second(), DEC);
  // Serial.println();
  // #endif

  delay(100);

  //checkrtc();

  if(interrecord == true)
  {
    detachInterrupt(digitalPinToInterrupt(btnPin));
    interupload = false;

    #ifdef DEBUG
    Serial.println("woke due to button!");  
    #endif
    
    logSupportfiles();
    startRec();
    interrecord = false;
  }

  if(interupload == true)
  {
    interrecord = false;

    #ifdef DEBUG
    Serial.println("woke due to rtc!");  
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

    checkTime2();  
    interupload = false;
  }

  detachInterrupt(digitalPinToInterrupt(btnPin));
}

/*
Crosscheck time for Uploads
*/
void checkrtc(){
  //  if(now.hour() == 20 && now.minute() == 1 && now.second() == 0){
  //    Serial.println("Supposed to Upload Now");
  //    interupload = true;
  //  }
  if(now.hour() == 16 && now.minute() == 0 && now.second() == 0){
     Serial.println("Supposed to Upload Now");
     interupload = true;
   }
  if(now.hour() == 7 && now.minute() == 0 && now.second() == 0){
     Serial.println("Restarting Unit");
     delay(200);
     restartDevice();
   }
}

/*
 * Interrupt handler for button Press
*/
void wakeUp()
{
    // Just a handler for the pin interrupt.
    #ifdef DEBUG
    Serial.println("Button Pressed!");
    #endif
    delay(200);
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