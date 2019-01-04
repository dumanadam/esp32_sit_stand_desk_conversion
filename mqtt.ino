int extractor(char sentTopic[], byte* sentPayload, unsigned int sentLength){
 
  int newLength = sentLength;

  Serial.println(""); 
  Serial.print("Message arrived [");
  Serial.print(sentTopic);
  Serial.println("] ");
  
  for (int i=0;i<sentLength;i++){
    mqttIn[i] = (char)sentPayload[i];
  }

  if ((mqttIn[0] < '0' or  mqttIn[1] != ',')){
    Serial.print("you shouldnty see this");
    } else {
    deskDirection = mqttIn[0]- '0';
    memset(mqttIn, 0, sizeof(mqttIn));
   /* mqttIn[newlength +1] = '\0';*/
    newLength -=2;
    for (int i=0; i<newLength; i++) {
      mqttIn[i] = (char)sentPayload[i+2];
      }  

    deskTimer = atoi(mqttIn);
    if (deskTimer > maxDeskTimer ) deskTimer = 10;

    
    Serial.println(deskDirection);
    Serial.println(deskTimer);
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  
  extractor(topic, payload, length);
  
  if (deskDirection == 0){
    // Stop the DC motor
    Serial.println("Motor stopped");
    motor("stop");
 
    }
    if (deskDirection == 1){
        Serial.println("Received desk_up");
        motor("up");
        statusCheck(2);
      }
    if (deskDirection == 2){
      Serial.println("Received desk DOWN");
      motor("down");
      statusCheck(2);
    }
    if (deskDirection == 3){
      Serial.println("Received desk TOP");
      motor("top");
      statusCheck(3);
      }
    if (deskDirection == 4){
      Serial.println("Received desk Bot");
      motor("bot");
      statusCheck(4);
      }
    if (deskDirection == 'r'){
      Serial.println("Received reset");
      resetEEPROM();
      statusCheck(4);
      }  
 
}

void statusCheck(byte msg) {
  if (msg == 1) {
    deskStatus[0] = '1';
    deskStatus[1] = '\0';
    client.publish(outStatus, deskStatus);
    Serial.println(" Error Detected, Deep sleep ON, errorStatus Set");
    esp_deep_sleep_start();
  } else if (msg == 2) {
    Serial.println(" Staus Check 2, Desk inbetween - Manual Mode,  MQTT 2 Sent");
    deskStatus[0] = '2';
    deskStatus[1] = '\0';
    EEPROM.put(1,(byte)deskStatus[0]);
    client.publish(outStatus, deskStatus);
  } else if (msg == 3) {
    Serial.println(" Staus Check 3, Desk Top, MQTT 3 Sent");
    deskStatus[0] = '3';
    deskStatus[1] = '\0';
    EEPROM.put(1,(byte)deskStatus[0]);
    client.publish(outStatus, deskStatus);
  } else if (msg == 4) {
    Serial.println(" Staus Check 4, Desk bot, MQTT 4 Sent");
    deskStatus[0] = '4';
    deskStatus[1] = '\0';
    EEPROM.put(1,(byte)deskStatus[0]);
    client.publish(outStatus, deskStatus);
  } else if (msg == '5') {
    Serial.println(" Staus Check 5, Desk reset request, MQTT 5 Sent");
    deskStatus[0] = '5';
    deskStatus[1] = '\0';
    EEPROM.put(1,(byte)deskStatus[0]);
    client.publish(outStatus, deskStatus);
  }
  else { 
    Serial.println("Error Status 0");
    errorStatus = 0;
    deskStatus[0] = 0;
    deskStatus[1] = '\0';
    EEPROM.put(1,deskStatus);
    client.publish(outStatus, deskStatus);
    EEPROM.get(1,deskStatus);
    Serial.print("EEPROM get is");
    Serial.println(deskStatus);
    
  }
  EEPROM.end();
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnnecting to ");
    Serial.print(broker);
    if (client.connect("desk")){
      Serial.println("\nSuccess! Connected to broker!");
      client.subscribe(inTopic);
    } else {
      Serial.println("Trying again in 5 seconds");
      delay(5000);
      }
      }
}
