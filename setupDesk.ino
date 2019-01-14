void setupDesk(){
  char deskPrev;
  int timedCycle;
  EEPROM.begin(EEPROM_SIZE);
  stateBotS= digitalRead(botLimitSwitchS);
  EEPROM.get(0,deskPrev);
  if (deskPrev == magicChar[0]) {
    Serial.println("magicchar match");
    eepStatusSave('r');
     if (deskStatus[0] >= '1' and deskStatus[0] <= '3') {
    Serial.println(" eepstatussave 3");
    statusCheck(3);
  } else if (deskStatus[0] == '4') {
    Serial.println("eepstatussave 4");
    statusCheck(4);
  } else {
    Serial.println("you fucked up");
    }
  } else if (stateBotS == HIGH) {
    statusCheck(1);
  } else {
    Serial.print("setup cycle");
    cycleTime = cycleTimer();
    deskPosInMilli = cycleTime;
    Serial.print("CycleTime in setup is ");
    Serial.println(cycleTime); 
    updateCycleTimer(cycleTime);
    deskPosP = 100; /* set position % as 100% */
    statusCheck(3);
  }    
  
}

void resetEEPROM(){
  EEPROM.begin(EEPROM_SIZE);
  for (int i=0; i<=EEPROM_SIZE; i++){
    EEPROM.write(i,255);
  }
  EEPROM.end();
}

void serialDebug (){
  int storedDeskStatus, storedCycleTime, storedDeskPosInMilli;
  int f;
  /*char w[6];
  w[0] = '2';
  w[1] = '\0';/*
  
  EEPROM.put(1,w);  /* w works */
  /*Serial.print(" put ");
  Serial.print(w);
  Serial.println(" in slot 1  deskstatus");*/
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, storedMagic);
  EEPROM.get(10, storedDeskStatus);
  EEPROM.get(20, storedCycleTime);
  EEPROM.get(30, storedDeskPosInMilli);
  Serial.print("storedMagic  from mem 0 is");
  Serial.println(storedMagic);
  Serial.print("storedDeskStatus from mem 10 is");
  Serial.println(storedDeskStatus);
  Serial.print("storedCycleTime from mem 20 is");
  Serial.println(storedCycleTime); 
  Serial.print("deskPosInMilli from mem 30 is");
  Serial.println(deskPosInMilli); 

  Serial.println(" ------");

  Serial.print(" No conversion - GET");
  Serial.println(" ");
  for (int i=0; i<=35; i++){
    EEPROM.get(i,f);
    Serial.print(i);
    Serial.print(" is ");
    Serial.print(f);
    Serial.print(" ");
}
  Serial.println(" ----");
  Serial.print(" No conversion READ - DEC");
  Serial.println(" ");
  for (int i=0; i<=35; i++){
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
    Serial.println("motor going bot"); 
    while (((millis() - startTime) < maxMotorTime) && stateBotS == LOW) {
      stateBotS= digitalRead(botLimitSwitchS);
    }
    motorMove("stop");
    startTime = millis();                   /* to switch from going down to up or vice versa*/
    motorMove("up"); 
    Serial.println("motor debouncing bot"); 
    while (((millis() - startTime) < 1000)) {
    }
    startT = millis();
    Serial.println("Timing full cycle - Top cycle" );
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going top"); 
    while ((millis() - startTime) < safeTopTime) {
  
    }
    motorMove("stop");
    cycleT = millis();
    diff = (cycleT - startT); /*cycle time from bot to top based on safeTopTime*/
    Serial.println(" ");
    Serial.print("Cycle Time  ");
    Serial.println(diff);
    Serial.println(" ");
    return diff;
  }

  void updateCycleTimer(int cycTime){
    EEPROM.begin(EEPROM_SIZE);
    Serial.print("Cycle Time in updateCycleTimer ");
    Serial.println(cycTime);
    Serial.println(" ");
    EEPROM.put(0,magicChar);
    EEPROM.put(20,cycTime);
    EEPROM.put(30,deskPosInMilli);
    EEPROM.end(); 
  }



