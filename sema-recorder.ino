#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_USE_GPRS true
#define DEBUG
// #define DUMP_AT_COMMANDS

// GPRS credentials
const char apn[]      = "internet";

// device details
String deviceId =  "SEMA1";

//uploading metrics
bool uploadingTime = false;
int uploadFails = 0;
int upName = 0;
int UpStatus = 0;
int updBL = 0;
String upDate = "";
byte buff[1024];
bool x;

//config metrics
bool loadedConfigs = false;
bool configExists = false;
int lastRecordedId = 0;
int lastUploadedID = 0;

//upload server url
const char server[]   = "noise-sensors-dashboard.herokuapp.com";
const char resource[] = "/audio/";
const int portx       = 80;

#include <TinyGsmClient.h>
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#define SD_ChipSelectPin 53
TMRpcm audio;

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
const byte gsm = 10;

String recName = "1.wav";
int recPin = A0;
int recFreq = 16000;
volatile bool recStarted = false;
bool recStartFlag = false;

int button_presses=1;

//timing metrics
unsigned long recStartTime = 0UL;
unsigned long recDuration = 10000UL ;

int ledPin = 13;
int btnPin = 18;

void setup() {

  SerialMon.begin(115200);
  delay(10);

  #ifdef DEBUG
  SerialMon.println(" SETUP SEQUENCE START!");
  #endif

  pinMode(A0, INPUT);
  pinMode(gsm, OUTPUT); //gsm
  pinMode(ledPin, OUTPUT); //led
  pinMode(btnPin, INPUT_PULLUP); //button

  digitalWrite(ledPin, LOW);
  attachInterrupt(digitalPinToInterrupt(btnPin), startRec, RISING);

  SD.begin(SD_ChipSelectPin);
  audio.CSPin = SD_ChipSelectPin;

  //setupTime();

  #ifdef DEBUG
  SerialMon.println(" SETUP SEQUENCE DONE!");
  #endif
}

void loop() {

  stopRec() ;
  
}

/*
 * Function to Start Recording 
*/
void startRec() {
  //recName=String(button_presses)+".wav";
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
    SerialMon.println("recording started 10s");
  }

  if ( (recStarted == true ) && ( (  millis() - recStartTime ) > recDuration) ) {

    audio.stopRecording( recName.c_str()) ;
    recStartFlag = false;
    recStarted = false;  
    button_presses++;
    recName=String(button_presses)+".wav";
    
    digitalWrite(ledPin, LOW);
    SerialMon.println("rec stoppped");
    SerialMon.println("next file record: "+recName);
    delay(3000);

    if(button_presses > 5){
     
       serverUpload();
      for (int i=1; i<6; i++){
      SerialMon.print("Uploading rec");  SerialMon.println(i);
         x = uploadWavFile(i, "60","2022-09-10T12:15" , deviceId);  
          if (x == true) {
          SerialMon.println("PASSED: Upload ");
          //return true;
        }
        else {
          SerialMon.println("FAILED : Upload ");
        }   
      }
      button_presses = 1;  //reset btn counter   
    } 
  }
}
