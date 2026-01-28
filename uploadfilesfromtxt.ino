
//send audio files to the server
void checkTime2() {
  #ifdef DEBUG
  SerialMon.println("---Uploading Feedback--");
  #endif
  ledOnSequence();
  SerialMon.println("test 1");
  // enableCommunication();
  // SerialMon.println("test 2");
  SdCardSequence();
  SerialMon.println("test 3");

  SerialMon.print("Button presses--");
  SerialMon.println(button_presses);

  //put code to manage uploads here: check 1.txt for timestamp
  //run for loop with counter for files
  //reset file counter if all files are sent to the server
  for (int i=1; i<button_presses; i++){

    //acquire support values for uploading api
    supportfilename = String(i)+".txt";
    audiofilename = String(i)+".wav";
    finaltime = "";

    #ifdef DEBUG
    SerialMon.print("Uploading feedback file: ");  
    SerialMon.println(audiofilename);
    #endif

    File testFile = SD.open(supportfilename,FILE_READ);
    if (testFile.available()) {
      String line = testFile.readStringUntil('\n');
      line.trim();
      String timestamp = line;  
      Serial.println(timestamp);
      String year = timestamp.substring(0, 4);
      String month = timestamp.substring(5, 7);
      String day = timestamp.substring(8, 10);
      String hour = timestamp.substring(11, 13);
      String minute = timestamp.substring(14, 16);
      String second = timestamp.substring(17, 19);
      finaltime = year + "-" + month + "-" + day + "T" + hour + ":" + minute;  
      // Rest of your code here...
      testFile.close();
      } else {
        // Handle the case where the line doesn't contain the expected format
        Serial.println("Error: Line format is not as expected.");
      }

    String device = deviceId;
    String time_recorded = finaltime;
    SerialMon.print("Time Recorded: ");
    SerialMon.println(time_recorded);

    x = uploadWavFile(audiofilename, device, time_recorded);
    // x = uploadWavFileTest(audiofilename, device, time_recorded);
    // x = uploadWavFileTesting(audiofilename, device, time_recorded);
      
    if (x == true) {
      #ifdef DEBUG
      SerialMon.println("PASSED: Upload");
      #endif
      //clear txt and wav after uploading
      if (SD.exists(audiofilename)) {
        
        SD.remove(audiofilename);
        #ifdef DEBUG
        SerialMon.println("Deleted: "+audiofilename);
        #endif
      }
      if (SD.exists(supportfilename)) {
        
        SD.remove(supportfilename);
        #ifdef DEBUG
        SerialMon.println("Deleted: "+supportfilename);
        #endif
      }

      delay(200);
      
      //reset counter after all files on card are uploaded
      if(i == (button_presses - 1)){
        #ifdef DEBUG
        SerialMon.println("Reset Counter Now");
        #endif         
        button_presses = 1;  //reset btn counter
        EEPROM.update(0, button_presses);
      }
    }
    else {
      #ifdef DEBUG
      SerialMon.println("FAILED: Upload");
      #endif
      EEPROM.update(0, button_presses);    
    }   
  }

  #ifdef DEBUG
  SerialMon.println("---Uploading Audio Complete--");
  #endif

  #ifdef DEBUG
  SerialMon.println("Uploading Metrics Now");
  #endif

  voltage = readVoltage(); // Call the function
  SerialMon.print("Input Voltage = ");
  SerialMon.println(voltage, 2); // Print voltage with 2 decimal places
  uploadMetricsJson(voltage);

  #ifdef DEBUG
  SerialMon.println("---Uploading Metrics Complete---");
  #endif

  ledOffSequence();
  disableCommunication();
  
}