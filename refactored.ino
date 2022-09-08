//  upDate = midString(returnTime(), "]{", "}{");
//  #ifdef DEBUG
//      SerialMon.print("rawdate = ");
//      SerialMon.println(upDate);
//  #endif
//  String datex = dateReturn(upDate);
//  #ifdef DEBUG
//      SerialMon.print("cleandate = ");
//      SerialMon.println(datex);
//  #endif
//"22/09/01,19:04:45+12"
//    String fileName = "1.wav";
//
//    if (SD.exists(fileName)) {
//      SD.remove(fileName);
//      SerialMon.println("Deleted file from card");
//    }
//
//    delay(500);


//  if (x == "success" || x > "") {
//    
//    SerialMon.print("Result: ");
//    SerialMon.println(x);
//
//    delay(1000);
//
//    uploadFails = 0;
//    bool updated_configs = false;
//
////    lastRecordedId = lastRecordedId;
////    lastUploadedID = lastUpID;
//
////    while (updated_configs == false) {
////
////      updateUploadStatus (lastUpID, _db.toInt(), dat);
////      updated_configs = updateConfigs( lastRecordedId, lastUploadedID );
////
////      delay(200);
////    } 
//    
//    #ifdef DEBUG
//    //SerialMon.print(lastRecordedId);
//    //SerialMon.print("--rec--up--");
//    SerialMon.print("Successfully uploaded");
//    //SerialMon.println(lastUploadedID);
//    #endif
//
////    updateLasRecAndUP(lastRecordedId, lastUploadedID);
//    return true;
//  } 
//  else {
//  
//    modem.maintain();
//
//    SerialMon.println("No reponse gotten from server");
//
//    uploadFails++;
//
//    if (uploadFails > 4) {
//      delay(2000);
//
//      SerialMon.println("Uploads failing : Reset");
//    }
//    return false;
//  }


//  #ifdef DEBUG
//  SerialMon.print("Modem Response Starts Here > ");
//  SerialMon.println( Modemresponse);
//  SerialMon.println(" Modem Response Ends here >");
//  #endif
//
//  String x = Modemresponse.substring( (Modemresponse.length() - 9), (Modemresponse.length() - 2));
//  SerialMon.println("response x =" + x);
