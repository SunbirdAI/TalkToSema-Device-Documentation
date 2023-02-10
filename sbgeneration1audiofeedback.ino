#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_USE_GPRS true
#define DEBUG
// #define DUMP_AT_COMMANDS

// GPRS credentials
const char apn[] = "internet";

// device details
//String deviceId =  "SEMA1";
String deviceId =  "TESTUNIT";

//uploading metrics
bool uploadingTime = false;
int uploadFails = 0;
int upName = 0;
int UpStatus = 0;
int updBL = 0;
String upDate = "";
byte buff[1024];
bool x;

//upload server url
const char server[]   = "sunbird-sema-django-env.eba-yxmbx6t2.eu-west-1.elasticbeanstalk.com";
const char resource[] = "/audio/";
const int portx       = 80;

#include <EEPROM.h>
#include <TinyGsmClient.h>
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>

#define SD_ChipSelectPin 53
TMRpcm audio;

#include <arduino-timer.h>
auto timer = timer_create_default();

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

TinyGsmClient client(modem);

int audiofile = 0;
unsigned long i = 0;
bool recmode = 0;
String recName = "1.wav";
int recFreq = 16000;
volatile bool recStarted = false;
bool recStartFlag = false;

int button_presses = 1;

/*
 * timing metrics
 * 21000UL --- DEPLOYMENT
 * 5000UL  --- TESTING
*/
unsigned long recStartTime = 0UL;
unsigned long recDuration = 5000UL; 

int gsmPin = 10;
int recPin = A0;
int ledPin = 13;
int btnPin = 18;

void (*resetFunc)(void) = 0;

void setup() {

  SerialMon.begin(115200);
  delay(1000);

  #ifdef DEBUG
  SerialMon.println("Loading Operating System");
  #endif

  /*
   * audio pin A0
   * gsm pin 10
   * led pin 13
   * button interrupt pin 18
  */
  
  pinMode(recPin, INPUT);          
  pinMode(gsmPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  pinMode(btnPin, INPUT_PULLUP); 

  digitalWrite(ledPin, LOW);

  /*
   * Read last button press from eeprom address 0
   * use it to guide on audio file to record
  */

  //EEPROM.update(0, 1);
  
  button_presses = EEPROM.read(0);

  if(button_presses > 5){
      //reset eeprom  only uncomment when wrong value in eeprom
      EEPROM.update(0, 1);
      button_presses == 1;
  }
  else
  {
    //otherwise read the value and continue recording
    recName = String(button_presses)+".wav";
  }
  
  #ifdef DEBUG
  SerialMon.print("eeprom value: ");
  SerialMon.println(button_presses);
  SerialMon.print("Next File: ");
  SerialMon.println(recName);
  #endif
  
  attachInterrupt(digitalPinToInterrupt(btnPin), startRec, RISING);
  
  if (!SD.begin(SD_ChipSelectPin)) {
    #ifdef DEBUG
    SerialMon.println("SD card Failure");
    #endif
  }
  else {
    #ifdef DEBUG
    SerialMon.println("SD card Success");
    #endif
  }

  /*
   * Restart Device Every Two Hours
  */
  timer.every(3600000, restartDevice);
  
  audio.CSPin = SD_ChipSelectPin;

  enableCommunication();

  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);

  #ifdef DEBUG
  SerialMon.println("System Boot Sequence Complete");
  #endif

  #ifdef DEBUG
  for (int r=1; r<6; r++){
    SerialMon.println("*");
    delay(1000);
  }
  #endif

  #ifdef DEBUG
  SerialMon.println("Voice Feedback Activated");
  #endif
  
}

void loop() {
  
  stopRec();
  timer.tick<void>();

}

void restartDevice(){
  #ifdef DEBUG
  SerialMon.println("Restarting Device --- Timer Elapsed");
  #endif
  digitalWrite(gsmPin, LOW);
  delay(5000);
  digitalWrite(gsmPin, HIGH);
  modem.init();
  resetFunc();
}

/*
 * Function to Start Recording 
*/
void startRec() {
  audio.startRecording(recName.c_str(), recFreq, recPin );
  recStarted = true;

}

/*
 * Function to Stop Recording 
 * keeps track of button presses
 * Initializes uploads when button presses greater than 5
*/
void stopRec() {

  if ( recStarted == true && recStartFlag == false ) {

    recStartTime = millis();
    recStartFlag = true;
    #ifdef DEBUG
    SerialMon.println("recording Started --- 20s");
    #endif
    digitalWrite(ledPin, HIGH);
  }

  if ( (recStarted == true ) && ( (  millis() - recStartTime ) > recDuration) ) {

    audio.stopRecording( recName.c_str()) ;
    recStartFlag = false;
    recStarted = false;  
    button_presses++;
    
    if(button_presses == 6){
      //LED START SEQUENCE
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
     
      for (int i=1; i<6; i++){

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
    
          delay(500);

          if(i == 5){
            
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

      //LED EXIT SEQUENCE
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200); 
    }

    EEPROM.update(0, button_presses);
    recName = String(button_presses)+".wav";
    
    digitalWrite(ledPin, LOW);
    
    #ifdef DEBUG
    SerialMon.println("recording Stoppped -- 20s");
    SerialMon.println("next File: "+recName);
    #endif
    
    delay(3000);
    
  }
}
