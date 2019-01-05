void setupDesk(){
  char deskPrev;
  int timedCycle;
  EEPROM.begin(EEPROM_SIZE);
  stateBotS= digitalRead(botLimitSwitchS);
  EEPROM.get(0,deskPrev);
  if (deskPrev == 818) {
    EEPROM.get(10, deskStatus);
    EEPROM.get(20, cycleTime);
  } else if (stateBotS == HIGH) {
    statusCheck(1);
  } else {
    timedCycle = cycleTimer();
    updateCycleTimer(timedCycle);
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
  int storedMagic, storedDeskStatus, storedCycleTime;
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
  Serial.print("storedMagic  from mem 0 is");
  Serial.println(storedMagic);
  Serial.print("storedDeskStatus from mem 10 is");
  Serial.println(storedDeskStatus);
  Serial.print("storedCycleTime from mem 20 is");
  Serial.println(storedCycleTime); 

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
    motor("bot");
    startT = millis();
    Serial.println("Timing full cycle - Top cycle" );
    motor("top");
    cycleT = millis();
    diff = cycleT - startT;
    Serial.println(" ");
    Serial.print("Cycle Time  ");
    Serial.println(diff);
    Serial.println(" ");
    return diff;
  }

  void updateCycleTimer(int cycTime){
    EEPROM.begin(EEPROM_SIZE);
    Serial.println(cycTime);
    Serial.println(" ");
    EEPROM.put(20,cycTime);
    EEPROM.put(0,magicChar);
    EEPROM.end(); 
  }

