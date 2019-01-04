void setupDesk(){
  char deskPrev;
  deskPrev = EEPROM.read(0);
  if (deskPrev == 120) {
    EEPROM.get(1, deskStatus);
    EEPROM.get(2, cycleTime);
  } else if (stateBotS == HIGH) {
    statusCheck(1);
  } else {
    Serial.println("Testing bottom Limit Switch");
    stateBotS= digitalRead(botLimitSwitchS);
    Serial.print("stateBots is ");
    Serial.println(stateBotS);
    stateBotS= digitalRead(botLimitSwitchS);
    motor("bot");
    startTime = millis();
    Serial.print("Timing full cycle - Top cycle" );
    Serial.println(startTime);
    motor("top");
    currentTime = millis();
    Serial.print("Current time is ");
    Serial.println(currentTime);
    cycleTime = currentTime - startTime;
    cycleTime = cycleTime / 1000;
    EEPROM.put(2,cycleTime);
    Serial.print("Cycle Time stores in mem 2 as ");
    Serial.println(cycleTime);
    statusCheck(3);
    }
}

void resetEEPROM (){
  for (int i=0; i<=EEPROM_SIZE; i++){
    EEPROM.write(i,255);
  }
  EEPROM.end();
}

