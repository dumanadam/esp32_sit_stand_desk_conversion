void setupDesk(){
  Serial.println("Testing bottom Limit Switch");
 /* startTime = millis();
  Serial.print("Start time is" );
  Serial.println(startTime);*/
  stateBotS= digitalRead(botLimitSwitchS);
  Serial.print("stateBots is ");
  Serial.println(stateBotS);
  stateBotS= digitalRead(botLimitSwitchS);
  if (stateBotS == HIGH) statusCheck(1); 
  motor("bot");
  
  /*while (looper == 1) {
    currentTime = millis();
    stateBotS= digitalRead(botLimitSwitchS);      
    if (stateBotS == HIGH or ((startTime - currentTime) > maxDeskTimer)) {
      motor("stop");
      Serial.println("Switch S = clicked"); 
      looper = 0;
      }
  }*/
  currentTime = millis();
  Serial.print("Current time is ");
  Serial.println(currentTime);
  cycleTime = millis() - currentTime;
  Serial.print("Cycle Time is ");
  Serial.println(cycleTime);
  
}

void statusCheck(byte msg) {
  if (msg == 1) {
    WiFi.mode( WIFI_MODE_NULL );
    errorStatus = 1;
    Serial.println(" Error Detected, Wifi Off, errorStatus Set");
  } else if (msg == 2) {
    Serial.println(" Staus Check 2, Desk inbetween - Manual Mode,  MQTT 2 Sent");
    deskStatus = "2";
    client.publish(outTopic, messages);
    client.publish(outStatus, deskStatus);
  } else if (msg == 3) {
    Serial.println(" Staus Check 3, Desk Top, MQTT 3 Sent");
    deskStatus = "3";
    client.publish(outTopic, messages);
    client.publish(outStatus, deskStatus);
  } else if (msg == 4) {
    Serial.println(" Staus Check 4, Desk bot, MQTT 4 Sent");
    deskStatus = "4";
    client.publish(outTopic, messages);
    client.publish(outStatus, deskStatus);
  } else if (msg == 5) {
    deskStatus = "5";
/*    snprintf(messages, , "Online",);*/
    client.publish(outTopic, messages);
    client.publish(outStatus, deskStatus);
  } else {
    errorStatus = 0;}
  }
