void uploadMetrics(float voltage) {

  #ifdef DEBUG
  SerialMon.print("Connecting to "); SerialMon.println(nserver);
  #endif

  // if (modem.isGprsConnected()) { SerialMon.println("Internet Connected!"); }

  client.connect(nserver, nport);

  // #ifdef DEBUG
  // SerialMon.println("serverConnection - successful");
  // #endif
 
    // Create JSON payload
  String postData = "{";
  postData += "\"device\":\"" + String(device_name) + "\",";
  postData += "\"temperature\":0,";
  postData += "\"pressure\":0,";
  postData += "\"humidity\":0,";
  postData += "\"air_quality\":0,";
  postData += "\"ram_value\":0,";
  postData += "\"system_temperature\":0,";
  postData += "\"power_usage\":" + String(voltage, 2);
  postData += "}";

  SerialMon.println(postData);

  // Send HTTP POST request
  client.print(String("POST ") + nresource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + nserver + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Content-Length: " + String(postData.length()) + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print(postData);

  #ifdef DEBUG
  SerialMon.println("Metrics Uploaded");
  #endif

  delay(2000);
  // String Modemresponse = "";
  // while (client.available()) {
  //   char a = (char) client.read();
  //   Modemresponse += a;
  // }
  client.stop();

  // Modemresponse.trim();
  // #ifdef DEBUG
  // SerialMon.println("Modem Response Starts Here > ");
  // SerialMon.println( Modemresponse);
  // SerialMon.println(" Modem Response Ends here >");
  // #endif
  client.stop();

  #ifdef DEBUG
  SerialMon.println("Server disconnected");
  #endif
  delay(100);
}

void uploadMetricsJson(float voltage)
{
  doc.clear();
  doc["device"] = device_name;
  doc["temperature"] = 0.0;
  doc["pressure"] = 0.0;
  doc["humidity"] = 0.0;
  doc["air_quality"] = 0.0;
  doc["ram_value"] = 0.0;
  doc["system_temperature"] = 0.0;
  doc["power_usage"] = voltage;


  sendtoserver = "";
  serializeJson(doc,sendtoserver);
  delay(4000);
  SerialMon.println(sendtoserver);
  delay(1000);

  
  #ifdef DEBUG
  SerialMon.print("Connecting to "); SerialMon.println(nserver);
  #endif

  // if (modem.isGprsConnected()) { SerialMon.println("Internet Connected!"); }

  // #ifdef DEBUG
  // SerialMon.println("serverConnection - successful");
  // #endif
  #ifdef DEBUG
  SerialMon.println("start HTTP POST request...");
  #endif
  // Send HTTP POST request
  http.beginRequest();
  // http.connectionKeepAlive();  // Currently, this is needed for HTTPS

  http.post(nresource, "application/json", sendtoserver);
  http.endRequest();
  #ifdef DEBUG
  int status = http.responseStatusCode();
  SerialMon.println("end HTTP POST request.");
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  #endif
  // #ifdef DEBUG
  // String body = http.responseBody();
  // SerialMon.print(F("Response:"));
  // SerialMon.println(body);
  // #endif
}