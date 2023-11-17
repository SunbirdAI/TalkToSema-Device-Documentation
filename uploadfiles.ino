bool uploadWavFile(String fname, String deviceID, String timerecorded) {
  String device = deviceID;
  String time_recorded = timerecorded;
  String fileName = fname;

  File dataFile = SD.open(fileName,FILE_READ);
  delay(200);
  if (dataFile) {
  } else {
    Serial.println("Error Opening file");
    return false;
  }

  #ifdef DEBUG
  SerialMon.print("Connecting to "); SerialMon.println(server);
  #endif

  if (modem.isGprsConnected()) { SerialMon.println("Internet Connected!"); }

  client.connect(server, portx);

  #ifdef DEBUG
  SerialMon.println("serverConnection - successful");
  #endif
 
  //global boundary content for the requests
  String content = "";
  String boundary_end = "\r\n--boundary1--\r\n";
  client.print(F("POST /audio/ HTTP/1.1\r\n"));
  client.print(String("Host: ") + server + "\r\n");
  client.print("Content-Type: multipart/form-data; boundary=boundary1\r\n");
  //-----------------start time_recorded here---------------------------
  content += boundary_end;
  content += "Content-Disposition: form-data; name=\"time_recorded\"\r\n\r\n";
  content += time_recorded;
  content += boundary_end;
  //-----------------start device id here---------------------------
  content += boundary_end;
  content += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  content += device;
  content += boundary_end;
  //-----------------end device id here---------------------------
  //----------------start triggering_threshold here-----------------------
  // content += "Content-Disposition: form-data; name=\"triggering_threshold\"\r\n\r\n";
  // content += "0";
  // content += boundary_end;
  //-----------------end triggering_threshold 
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
  Modemresponse.trim();
  #ifdef DEBUG
  SerialMon.println("Modem Response Starts Here > ");
  SerialMon.println( Modemresponse);
  SerialMon.println(" Modem Response Ends here >");
  #endif
  delay(100);
  return true;
}