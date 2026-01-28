void updateRTCFromGSM(String gsmTime) {
    int day, month, year, hour, minute, second;

    // GSM time format: "25/02/25,17:19:21+12"
    if (gsmTime.length() < 17) {
        SerialMon.println("Invalid GSM time format!");
        return;
    }

    // Parse GSM time (DD/MM/YY,HH:MM:SS)
    sscanf(gsmTime.c_str(), "%d/%d/%d,%d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    // Convert 2-digit year to full year
    year += 2000;

    // Set RTC
    rtc.adjust(DateTime(year, month, day, hour, minute, second));

    SerialMon.println("RTC updated from GSM time!");
}

/*
 * Function to Enable 4G MODEM
*/
void enableCommunicationTime(){
  
    if (digitalRead(gsmPin) == LOW) {
      digitalWrite(gsmPin, HIGH);
    }

    delay(60000); //give 6s to gsm to start fully
    
    SerialAT.begin(115200);

    #ifdef DEBUG
    SerialMon.println("Initializing modem");
    #endif
    
    // modem.init();
    
    if (modem.isNetworkConnected()) {
      #ifdef DEBUG
      SerialMon.println("Network connected");
      #endif
    }
    else {
      SerialMon.println("Network Failed");
    }
}