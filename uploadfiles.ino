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

  // if (modem.isGprsConnected()) { SerialMon.println("Internet Connected!"); }

  client.connect(server, portx);

  // #ifdef DEBUG
  // SerialMon.println("serverConnection - successful");
  // #endif
 
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

  SerialMon.println(content);

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

  String uploadstate = Modemresponse.substring( (Modemresponse.length() - 9), (Modemresponse.length() - 2));

  #ifdef DEBUG
  SerialMon.println("Upload state x = " + uploadstate);
  #endif

  if (uploadstate == "success" || uploadstate > "") {
  // if (uploadstate == "success") {
    return true;
  }else {
    return false;
  }

 delay(100);
//  return true;
}

bool uploadWavFileTest(String fname, String deviceID, String timerecorded) {
    // String boundary = "boundary1";
    // String boundary_start = "--boundary1\r\n";
    // String boundary_end = "\r\nboundary1--";
    String boundary_end = "\r\n--boundary1--\r\n";

    File dataFile = SD.open(fname, FILE_READ);
    if (!dataFile) {
        Serial.println("Error Opening file");
        return false;
    }

    #ifdef DEBUG
        SerialMon.print("Connecting to ");
        SerialMon.println(server);
    #endif

    // Construct multipart form-data content
    String payload = "";
    payload += boundary_end;
    payload += "Content-Disposition: form-data; name=\"time_recorded\"\r\n\r\n" + timerecorded;
    payload += boundary_end;
    payload += "Content-Disposition: form-data; name=\"device\"\r\n\r\n" + deviceID;
    payload += boundary_end;
    payload += "Content-Disposition: form-data; name=\"audio\"; filename=" + fname + "\r\n";
    payload += "Content-Type: audio/wav\r\n\r\n";

    // Calculate content length
    size_t contentLength = payload.length() + dataFile.size() + boundary_end.length() +"\r\n";

    SerialMon.println(payload);
    SerialMon.println(contentLength);

    // Start HTTP request
    http.beginRequest();
    http.post("/audio/");

    // client.print("Content-Type: multipart/form-data; boundary=boundary1\r\n");
    // Send headers
    http.sendHeader("Content-Type", "multipart/form-data; boundary= boundary1\r\n");
    http.sendHeader("Content-Length", String(contentLength));

    // **Start the body of the request**
    http.beginBody();

    // Send payload headers
    http.print(payload);

    // Send file data in chunks
    // uint8_t buff[1024];
    while (dataFile.available()) {
        size_t readBytes = dataFile.read(buff, sizeof(buff));
        http.write(buff, readBytes);
    }
    dataFile.close();

    // Send closing boundary
    http.print(boundary_end);

    http.endRequest(); // Finalize the request

#ifdef DEBUG
    int statusCode = http.responseStatusCode();
    SerialMon.print("Response status code: ");
    SerialMon.println(statusCode);
#endif

    return (http.responseStatusCode() == 200); // Return true if upload is successful
}

bool uploadWavFileTesting(String fname, String deviceID, String timerecorded) {
    File dataFile = SD.open(fname, FILE_READ);
    if (!dataFile) {
        Serial.println("Error Opening file");
        return false;
    }

#ifdef DEBUG
    SerialMon.print("Connecting to ");
    SerialMon.println(server);
#endif

    // Calculate content length (only file size, since we send raw binary)
    size_t contentLength = dataFile.size();

    // Start HTTP request
    http.beginRequest();
    http.post("/audio/test_version/");

    // Send headers
    http.sendHeader("Content-Type", "application/octet-stream");
    http.sendHeader("Content-Length", String(contentLength));
    http.sendHeader("time_recorded", timerecorded);
    http.sendHeader("device", deviceID);

    // **Start the body of the request**
    http.beginBody();

    // Send file data in chunks
    // uint8_t buff[1024];
    // while (dataFile.available()) {
    //     size_t readBytes = dataFile.read(buff, sizeof(buff));
    //     http.write(buff, readBytes);
    // }
    // dataFile.close();

    if (dataFile) {
      int i = 0;
      while (dataFile.available() > 0) {
        buff[i] = dataFile.read();     //BINARY
        i++;
        if (i == 1024) {
          http.write(buff, i);
          i = 0;
          memset(buff, 0, sizeof(buff));
        }
      }
      http.write(buff, i);
      memset(buff, 0, sizeof(buff));
      dataFile.close();
    }

    http.endRequest(); // Finalize the request

#ifdef DEBUG
    int statusCode = http.responseStatusCode();
    SerialMon.print("Response status code: ");
    SerialMon.println(statusCode);
#endif

    return (http.responseStatusCode() == 200); // Return true if upload is successful
}
