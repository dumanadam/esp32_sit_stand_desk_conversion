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
    eepStatusSave('r', "Incorrect MQTT format");
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
  int percCalc;
  
  extractor(topic, payload, length);
  

    if (deskDirection == 0){
      motor("stop");
      } else if (deskDirection == 1 and (deskPosInMilli + deskTimer) <= safeTopTime ){
        percTarget = ((((float)deskPosInMilli+(float)deskTimer)/(float)cycleTime)*100);
        Serial.print("callback percTarget = " );
        Serial.println(percTarget);
        motor("up");
        statusCheck(1);
      } else  if (deskDirection == 2 and (deskPosInMilli- deskTimer) >= safeBotTime ){
        percTarget = ((((float)deskPosInMilli-(float)deskTimer)/(float)cycleTime)*100);
        Serial.print("callback percTarget = " );
        Serial.println(percTarget);
        motor("down");
        statusCheck(2);
      } else if (deskDirection == 3 and deskPosInMilli < safeTopTime ){
        percTarget =100;
        motor("up");
        statusCheck(3);
      } else if (deskDirection == 4 and deskPosInMilli > safeBotTime ){
        percTarget = 1;
        motor("down");
        motor("debounceBot");
        statusCheck(4);
      } else if (deskDirection == 5){
        if (deskTimer > 100)  deskTimer = 100;
        if (deskTimer < 1) deskTimer = 1;
        percCalc = deskTimer - deskPosPerc();
        percTarget = deskTimer;
        percCalc < 0 ? motor("down"): motor("up");
        statusCheck(5);
      } else if (deskDirection == 6){
        Serial.println("Received Debug");
        statusCheck(6);
      } else if (deskDirection == 7){
        statusCheck(7);
      } else {
        eepStatusSave('s', "Time out of bounds");  
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
    deskPosP = deskPosPerc();
    deskStatus[0] = '5';
    deskStatus[1] = '\0';
    eepStatusSave('s'); 
  } else if (msg == 6) {
    eepStatusSave('s', "Serial Debug Requested");
    Serial.println(" Debug Requested");
    serialDebug();
  } else if (msg == 7) {
    eepStatusSave('s', "EEPROM Reset Requested");
    resetEEPROM();
  } else if (msg == 8) {
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

void movePerc(int recPerc) { 
  int origRecPerc = recPerc, breaker = 0;
  int peo, startTime = millis(), deo = deskPosInMilli, counter = 0;
  char temp[50];
  
  recPerc -= deskPosP;                                                  /* find difference in received target % to current desk position % */
  do {
    int timer = (millis() - startTime), de = deskPosInMilli, pe;      /* timer  */
    de = (recPerc < 0) ? de - timer: de + timer;                      /* If negative go down, if positive go up*/
    pe = ((float)de / (float)cycleTime) * 100;                        /* pe is position % based on time passed since loop started */
    peo=pe;
    sprintf(temp,"we have moved 1% = %d", peo );
    if (pe-(deskPosP + counter) >=1 ) {                               /* compare % position and only publish every 1 % point */
      Serial.println(temp);
      sprintf(outPayload, "%d", pe);
      client.publish(outPerc, outPayload);
      counter++;         
    }
    if ((deskPosP + counter)-pe >= 1 ) {
      Serial.println(temp);
      sprintf(outPayload, "%d", pe);
      client.publish(outPerc, outPayload);
      counter--;         
    }  
    de = deskPosInMilli;
    stateBotS= digitalRead(botLimitSwitchS); 
    
    if(deskDirection == 4 or deskDirection == 2){
      if (stateBotS == HIGH and pe < 10) {
        breaker =1; /* hack to get end stop to work, triggering for some reason*/
        eepStatusSave('r', "stateBotS triggered");
      }
    }
  } while (peo!= origRecPerc && peo <= 100 && peo >=1  && breaker == 0);
 }
 
void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnnecting to ");
    Serial.print(broker);
    if (client.connect(brokerClient, brokerUser, brokerPass)){
      Serial.println("\nSuccess! Connected to broker!");
      client.subscribe(inTopic);
      eepStatusSave('r');
    } else {
      Serial.println("Trying again in 5 seconds");
      delay(5000);
      }
      }
}

