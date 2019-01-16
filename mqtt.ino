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
    eepStatusSave('s', "Incorrect MQTT format");
    } else {
    deskDirection = mqttIn[0]- '0';
    memset(mqttIn, 0, sizeof(mqttIn));
    newLength -=2;
    for (int i=0; i<newLength; i++) {
      mqttIn[i] = (char)sentPayload[i+2];
    }
    deskTimer = atoi(mqttIn);
    if (deskTimer > maxMotorTime ) deskTimer = maxMotorTime;

  }
}

void callback(char* topic, byte* payload, unsigned int length){
  
  extractor(topic, payload, length);
  

    if (deskDirection == 0){
      motor("stop");
      } else if (deskDirection == 1 and (deskPosInMilli + deskTimer) <= safeTopTime ){
        motor("up", deskTimer);
      } else  if (deskDirection == 2 and (deskPosInMilli- deskTimer) >= safeBotTime ){
        motor("down", deskTimer);
      } else if (deskDirection == 3 and deskPosInMilli < safeTopTime ){
        motor("top");
      } else if (deskDirection == 4 and deskPosInMilli > safeBotTime ){
        motor("bot");
      } else if (deskDirection == 5){
        Serial.println("Received Debug");
        statusCheck(5);
      } else if (deskDirection == 6){
        statusCheck(6);
      } else {
        eepStatusSave('s', "Time sent is out of bounds");  
      }   
}

void statusCheck(byte msg) {

  EEPROM.begin(EEPROM_SIZE);
  if (msg == 1) {
    deskPosP = deskPosPerc();    
    deskStatus[0] = '1';
    deskStatus[1] = '\0';
    eepStatusSave('s');
  } else if (msg == 2) {
    deskPosP = deskPosPerc();
    deskStatus[0] = '2';
    deskStatus[1] = '\0';
    eepStatusSave('s');  
  } else if (msg == 3) {
    deskPosP = deskPosPerc();
    deskStatus[0] = '3';
    deskStatus[1] = '\0';
    eepStatusSave('s');
  } else if (msg == 4) {
    deskPosP = deskPosPerc();
    deskStatus[0] = '4';
    deskStatus[1] = '\0';
    eepStatusSave('s'); 
  } else if (msg == 5) {
    eepStatusSave('s', "Serial Debug Requested");
    Serial.println(" Debug Requested");
    serialDebug();
  } else if (msg == 6) {
    eepStatusSave('s', "EEPROM Reset Requested");
    resetEEPROM();
  } else if (msg == 7) {
    deskStatus[0] = '1';
    deskStatus[1] = '\0';
    eepStatusSave('s', " Error Detected, Deep sleep ON, errorStatus Set");
    esp_deep_sleep_start();
  }
  EEPROM.end();
}

int deskPosPerc() {
  int percentage = ((float)deskPosInMilli / (float)cycleTime) * 100;
  return percentage;
  }

void deskPosPercL(char direct[3]){
  int dp = deskPosInMilli, timer = (millis() - startTime), de = deskPosInMilli, perc, pe;
  if (direct == "up") de=de +timer;
  if (direct == "dn") de=de -timer;
  pe = ((float)de / (float)cycleTime) * 100;
  sprintf(outPayload, "%d", pe);
  client.publish(outPerc, outPayload);
  de = deskPosInMilli;
}


void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnnecting to ");
    Serial.print(broker);
    if (client.connect("desk")){
      Serial.println("\nSuccess! Connected to broker!");
      client.subscribe(inTopic);
      eepStatusSave('r');
    } else {
      Serial.println("Trying again in 5 seconds");
      delay(5000);
      }
      }
}

