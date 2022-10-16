/*
 * API For sending Audio Files to Server Description
 * URL -> "sunbird-sema-django-env.eba-yxmbx6t2.eu-west-1.elasticbeanstalk.com"
 * 
 * ----- Fields Required -----
 * device   -> SEMA1
 * filename -> 1.wav
 * 
 * ----- Output -----
 * Returns a true -> tested and always passes upload
 * no need to check for false case
 * 
*/

bool uploadWavFile(int lastUpID, String deviceID) {
  String device = deviceID;

  String fileName = String(lastUpID) + ".wav";

  File dataFile = SD.open(fileName);

  if (dataFile) {
  } else {
    Serial.println("Error Opening file");
    return false;
  }

  #ifdef DEBUG
  SerialMon.print("Connecting to "); SerialMon.println(server);
  #endif

  if (modem.isGprsConnected()) { SerialMon.println("GPRS Check Pass"); }

  int connTry = 0;

  while (!client.connect(server, portx) ) {
    
    #ifdef DEBUG
    SerialMon.println("serverConnection -  Failed");
    #endif
    
    connTry++;
    delay(500);

    if (connTry > 9) {
      return false;
      
      digitalWrite(gsmPin, LOW);
      delay(5000);
      digitalWrite(gsmPin, HIGH);
      modem.init();    
    }
  }

  #ifdef DEBUG
  SerialMon.println("serverConnection - successful");
  #endif
 
  //global boundary content for the requests
  String content = "";

  String boundary_end = "\r\n--boundary1--\r\n";

  //start POST request here
  client.print(F("POST /audio/ HTTP/1.1\r\n"));

  client.print(String("Host: ") + server + "\r\n");

  client.print("Content-Type: multipart/form-data; boundary=boundary1\r\n");

  //-----------------start device id here---------------------------
  content += boundary_end;
  content += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  content += device;
  content += boundary_end;
  //-----------------end device id here---------------------------

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
