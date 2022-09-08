/*
 * API For sending Audio Files to Server Description
 * URL -> https://noise-sensors-dashboard.herokuapp.com/
 * 
 * ----- Requirements -----
 * time_recorded -> 2022-09-10T121500
 * triggering_threshold -> 60
 * device -> SEMA1
 * filename -> 1.wav
 * 
 * --- Output -----
 * Returns a true -> tested and always passes upload
 * 
*/


bool uploadWavFile(int lastUpID, String _db, String dat, String deviceID) {
  String device = deviceID;

  String time_recorded = dat;

  String triggering_threshold = _db;

  String fileName = String(lastUpID) + ".wav";

  File dataFile = SD.open(fileName);

  if (dataFile) {
  } else {
    Serial.println("Error Opening file");
    return false;
  }

  SerialMon.print("Connecting to "); SerialMon.println(server);

  if (modem.isGprsConnected()) { SerialMon.println("GPRS Check Pass"); }

  int connTry = 0;

  while (!client.connect(server, portx) ) {
   
    SerialMon.println("serverConnection -  Failed");
    
    connTry++;
    delay(500);

    if (connTry > 4) {
      return false;
    }
  }
  
  SerialMon.println("serverConnection - successful");
 
  //global boundary content for the requests
  String content = "";

  String boundary_end = "\r\n--boundary1--\r\n";

  //start POST request here
  client.print(F("POST /audio/ HTTP/1.1\r\n"));

  client.print(String("Host: ") + server + "\r\n");

  client.print("Content-Type: multipart/form-data; boundary=boundary1\r\n");


  //-----------------start time_recorded here---------------------------
  content += boundary_end;
  content += "Content-Disposition: form-data; name=\"time_recorded\"\r\n\r\n";
  content += time_recorded;
  content += boundary_end;
  //-----------------end time_recorded here---------------------------

  //-----------------start device id here---------------------------
  content += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  content += device;
  content += boundary_end;
  //-----------------end device id here---------------------------

  //----------------start triggering_threshold here---------------------------
  content += "Content-Disposition: form-data; name=\"triggering_threshold\"\r\n\r\n";
  content += triggering_threshold;
  content += boundary_end;
  //-----------------end triggering_threshold here---------------------------

  //-----------------send audio file content here---------------------------   +boundary_st.length()
  content += "Content-Disposition: form-data; name=\"audio\"; filename=" + fileName + "\r\n";
  String audiotype = "audio/wav";
  content += "Content-Type: " + audiotype + "\r\n\r\n";

  client.print("Content-Length: " + String(content.length() + dataFile.size() + boundary_end.length()) + "\r\n");

  client.print(F("Connection: close\r\n\r\n"));

  client.print(content);

  if (dataFile) {
    int i = 0;
    while (dataFile.available() > 0) {
      buff[i] = dataFile.read();     //BINARY
      i++;
      if (i == 1024) {
        client.write(buff, i);
        i = 0;
        memset(buff, 0, sizeof(buff));
      }
    }
    client.write(buff, i);
    memset(buff, 0, sizeof(buff));
    dataFile.close();
  }
  client.print(boundary_end);
  //-----------------audio file content ends here---------------------------

  //process server response here

  delay(2000);

  String Modemresponse = "";
  while (client.available()) {
    char a = (char) client.read();
    Modemresponse += a;
  }
  
  client.stop();
  delay(100);

  return true;
}
