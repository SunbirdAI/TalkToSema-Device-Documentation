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
    SerialMon.println("Initializing modem...");
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
