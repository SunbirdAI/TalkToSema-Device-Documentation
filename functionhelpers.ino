/*
 * Function to return NTP time from 4G chip 
*/
void setupTime() {
  modem.NTPServerSync("1.africa.pool.ntp.org", 12); // 12 is timezone +3 GMT
  delay(200);
  #ifdef DEBUG
  SerialMon.println("Time Updated successfully");
  #endif
}

/*
 * Function to capture time from 4G chip 
*/
String returnTime() {
  String timex = modem.getGSMDateTime(DATE_FULL);
  return timex;
}

/*
 * Function to Format Time according to API 
*/
String dateReturn(String x) {
  int x1 = x.indexOf('+');

  String Yrx =    x.substring( 0 , 2 );
  String Mox =    x.substring(3 , 5);
  String Dax =    x.substring(6 , 8);
  String hrx =    x.substring(9 , 11);
  String Minx =   x.substring(12 , 14);
  
  #ifdef DEBUG  
  SerialMon.print("Year :"); SerialMon.println(Yrx);
  SerialMon.print("Mox :");  SerialMon.println(Mox);
  SerialMon.print("Dax :");  SerialMon.println(Dax);
  SerialMon.print("hrx :");  SerialMon.println(hrx);
  SerialMon.print("Minx :"); SerialMon.println(Minx);
  #endif
  
  String finalx = "20" + Yrx + "-" + Mox + "-" + Dax + "T" + hrx + ":" + Minx;

  SerialMon.println("Final X: " + finalx );
  return finalx;
}

/*
 * String Extraction 
*/
String midString(String str, String start, String finish) {
  int locStart = str.indexOf(start);
  if (locStart == -1) return "";
  locStart += start.length();
  int locFinish = str.indexOf(finish, locStart);
  if (locFinish == -1) return "";
  return str.substring(locStart, locFinish);
}

/*
 * Function to Enable 4G Communication 
*/
void serverUpload(){
  
    if (digitalRead(gsm) == LOW) {
      digitalWrite(gsm, HIGH);
    }

    delay(10000); //give 6s to gsm to start fully
    SerialAT.begin(115200);

    #ifdef DEBUG
    SerialMon.println("Initializing modem...");
    #endif
    modem.init();
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network connected");
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
      SerialMon.println("GPRS connected");
    }
}
