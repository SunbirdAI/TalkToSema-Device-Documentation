/*
 * Function to Enable 4G Communication 
*/
void enableCommunication(){
  
    if (digitalRead(gsmPin) == LOW) {
      digitalWrite(gsmPin, HIGH);
    }

    delay(10000); //give 6s to gsm to start fully
    SerialAT.begin(115200);

    #ifdef DEBUG
    SerialMon.println("Initializing modem");
    #endif
    
    modem.init();
    
    if (modem.isNetworkConnected()) {
      #ifdef DEBUG
      SerialMon.println("Network connected");
      #endif
    }
    if (!modem.gprsConnect(apn)) {
      #ifdef DEBUG
      SerialMon.println("APN: fail");
      #endif
      delay(10000);
      return;
    }

    #ifdef DEBUG
    SerialMon.println("APN: internet");
    #endif

    if (modem.isGprsConnected()) {
      #ifdef DEBUG
      SerialMon.println("GPRS connected");
      #endif
    }
}

void disableCommunication(){
    if (digitalRead(gsmPin) == HIGH) {
      digitalWrite(gsmPin, LOW);
      delay(200);
    }
    digitalWrite(gsmPin, LOW);
    delay(200);
    
    #ifdef DEBUG
    SerialMon.println("Modem powered off");
    #endif  
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

//log support file for recording
void logSupportfiles(){
  dataStringtime = "";
  rtcstamp = now.timestamp(DateTime::TIMESTAMP_FULL);
  dataStringtime = dataStringtime + rtcstamp;
  String savingName  = String(button_presses)+".txt";

  #ifdef DEBUG
  SerialMon.print("Support file: "); SerialMon.println(savingName);
  #endif

  File dataFiley = SD.open(savingName, FILE_WRITE);
  if (dataFiley) {
    dataFiley.println(dataStringtime);
    dataFiley.close();

    #ifdef DEBUG
    SerialMon.println("data saved successfully");
    #endif
  }
  else {
    #ifdef DEBUG
    SerialMon.println("error opening" + savingName);
    #endif
  }
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

void SdCardSequence(){
  bool opencard = SD.begin(SD_ChipSelectPin);
  delay(5000);

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
}

void ledOnSequence(){
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPin, HIGH);
}

void ledOffSequence(){
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200); 
}