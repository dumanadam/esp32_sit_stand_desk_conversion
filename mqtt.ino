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
  
  extractor(topic, payload, length);
  

    if (deskDirection == 0){
      motor("stop");
      } else if (deskDirection == 1 and (deskPosInMilli + deskTimer) <= safeTopTime ){
        percTarget = ((((float)deskPosInMilli+(float)deskTimer)/(float)cycleTime)*100);
        Serial.print("callback percTarget = " );
        Serial.println(percTarget);
        motor("up", deskTimer);
      } else  if (deskDirection == 2 and (deskPosInMilli- deskTimer) >= safeBotTime ){
        percTarget = ((((float)deskPosInMilli-(float)deskTimer)/(float)cycleTime)*100);
              Serial.print("callback percTarget = " );
        Serial.println(percTarget);
        motor("down", deskTimer);
      } else if (deskDirection == 3 and deskPosInMilli < safeTopTime ){
        motor("top");
      } else if (deskDirection == 4 and deskPosInMilli > safeBotTime ){
        motor("bot");
      } else if (deskDirection == 5){
        movePerc(deskTimer);
      } else if (deskDirection == 6){
        Serial.println("Received Debug");
        statusCheck(6);
      } else if (deskDirection == 7){
        statusCheck(7);
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

void deskPosPercL(char direct[3]){
  int dp = deskPosInMilli, timer = (millis() - startTime), de = deskPosInMilli, pe, counter = 0;
  if (direct == "up") de=de +timer;
  if (direct == "dn") de=de -timer;
  pe = ((float)de / (float)cycleTime) * 100;
  if (pe-(deskPosP + counter) >=1  ) {
          Serial.print("sent outPerc payload");
          counter++;
  sprintf(outPayload, "%d", pe);
  client.publish(outPerc, outPayload);
  }
  de = deskPosInMilli;
}

 void movePerc(int recPerc) { /* add safety for up and down top and bot*/
  int origRecPerc = recPerc;
  int peo, startTime = millis(), deo = deskPosInMilli, counter = 0;
  char temp[50];
Serial.print("moveperc percTarget = " );
        Serial.println(percTarget);
        Serial.print("moveperc recperc= " );
        Serial.println(recPerc);
  if (recPerc == 0) {
    motor("bot");
  } else if (recPerc == 100) {
    motor("top");
  } else {
    recPerc -= deskPosP;     
       do {
          int timer = (millis() - startTime), de = deskPosInMilli, pe;
          de = (recPerc < 0) ? de - timer: de + timer;
        pe = ((float)de / (float)cycleTime) * 100;
        peo=pe;
     /*   Serial.print("recperc= ");
        Serial.print(recPerc);
        Serial.print(" de=");
        Serial.print(de);
        Serial.print(" pe=");
        Serial.print(pe);
        Serial.print(" peo=");
        Serial.print(peo);
        Serial.print(" deskPos{");
        Serial.println(pe-(deskPosP + counter));*/
        
       /* sprintf(outPayload, "%d", pe);
        client.publish(outPerc, outPayload);*/
        sprintf(temp,"we have gone down 1% = %d", peo );
        if (pe-(deskPosP + counter) >=1 ) {
          Serial.println(temp);
          counter++;         
        }
        if ((deskPosP + counter)-pe >= 1 ) {
          Serial.println(temp);
          counter--;         
        }  
        de = deskPosInMilli;
      } while (peo!= origRecPerc );
    }
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

