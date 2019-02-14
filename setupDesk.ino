void setupDesk(){
  char deskPrev;
  int timedCycle;
  EEPROM.begin(EEPROM_SIZE);
  stateBotS= digitalRead(botLimitSwitchS);
  EEPROM.get(0,deskPrev);
  if (deskPrev == magicChar[0]) {
    Serial.println("magicchar match");
    eepStatusSave();
  } else if (stateBotS == HIGH) {
    eepStatusSave('r', "Bot End Stop Error");
  } else {
    Serial.print("setup cycle");
    cycleTime = cycleTimer();
    deskPosInMilli = cycleTime;
    deskPosP = 100; /* set position % as 100% */
    statusCheck(3);
  }    
}

void resetEEPROM(){
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0,255);
  EEPROM.end();
  Serial.println("Reset EEPROM, rebooting in 3 seconds");
  eepStatusSave('r', "Reset EEPROM, rebooting in 3 seconds");
  delay(3000);
  ESP.restart();
}

void serialDebug (){
  int storedDeskStatus, storedCycleTime, storedDeskPosInMilli, f;
  
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, storedMagic);
  EEPROM.get(10, storedDeskStatus);
  EEPROM.get(20, storedCycleTime);
  EEPROM.get(30, storedDeskPosInMilli);
  EEPROM.get(40, deskPosP);
  Serial.print("storedMagic  from mem 0 is");
  Serial.println(storedMagic);
  Serial.print("storedDeskStatus from mem 10 is");
  Serial.println(storedDeskStatus);
  Serial.print("storedCycleTime from mem 20 is");
  Serial.println(storedCycleTime); 
  Serial.print("deskPosInMilli from mem 30 is");
  Serial.println(deskPosInMilli); 
  Serial.print("deskPosP from mem 40 is");
  Serial.println(deskPosP); 
  Serial.println(" ------");
  Serial.print(" No conversion - GET");
  Serial.println(" ");
  for (int i=0; i<=EEPROM_SIZE; i++){
    EEPROM.get(i,f);
    Serial.print(i);
    Serial.print(" is ");
    Serial.print(f);
    Serial.print(" ");
}
  Serial.println(" ----");
  Serial.print(" No conversion READ - DEC");
  Serial.println(" ");
  for (int i=0; i<=EEPROM_SIZE; i++){
    f = EEPROM.read(i);
    Serial.print(i);
    Serial.print(" is ");
    Serial.print(f);
    Serial.print(" ");
  }
  EEPROM.end();
}



int cycleTimer(){
   int startT,cycleT,diff;
    Serial.println("Testing bottom Limit Switch");
    Serial.println("");
    stateBotS= digitalRead(botLimitSwitchS);
    startTime = millis();
    motorMove("down"); 
    while (((millis() - startTime) < maxMotorTime) && stateBotS == LOW) {
      delay(10);
      stateBotS= digitalRead(botLimitSwitchS);
    }
    motorMove("stop");
    startTime = millis();                   
    motorMove("up"); 
    Serial.println("motor debouncing bot"); 
    while (((millis() - startTime) < 1000)) {
    }
    startT = millis();
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going top"); 
    while ((millis() - startTime) < safeTopTime) {
    }
    motorMove("stop");
    cycleT = millis();
    diff = (cycleT - startT)-1000; /*cycle time from bot to top based on safeTopTime*/
    return diff;
  }

  void updateCycleTimer(int cycTime){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0,magicChar);
    EEPROM.put(20,cycTime);
    EEPROM.put(30,deskPosInMilli);
    EEPROM.end(); 
  }


