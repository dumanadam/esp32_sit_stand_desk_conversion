
void motorMove (char motorMove[]) {
  if(motorMove == "stop") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, LOW); 
  } else if (motorMove == "up"){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, HIGH);
  } else if (motorMove == "down") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH); 
    digitalWrite(motor1Pin3, HIGH);
    digitalWrite(motor1Pin4, LOW); 
  } else  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, LOW); 
  }
  
}




void motor(char message[], int motorTimer = maxMotorTime) {
  
  stateBotS= digitalRead(botLimitSwitchS);
  startTime = millis();
  if(message == "stop") {
    motorMove("stop");
  } else if (message == "up"){
    startTime = millis();
    motorMove("up"); 
   /* while (((millis() - startTime) < motorTimer) && deskPosInMilli < safeTopTime ) {
      stateBotS= digitalRead(botLimitSwitchS);      
      /*deskPosPercL("up");*/
         Serial.print("motor up percTarget = " );
        Serial.println(percTarget);
      movePerc(percTarget);
         
   /* }*/
    motorMove("stop");
    deskPosInMilli += (millis() - startTime);
    statusCheck(1);
  } else if (message == "down") {
    startTime = millis();
    motorMove("down"); 
   /* while (((millis() - startTime) < motorTimer) && stateBotS == LOW ) {
      stateBotS= digitalRead(botLimitSwitchS);
      /*deskPosPercL("dn");*/
      Serial.print("motor down percTarget = " );
        Serial.println(percTarget);
      movePerc(percTarget);
      
    /*}*/
    motorMove("stop");
    deskPosInMilli -= (millis() - startTime);
    statusCheck(2);
   /* if (deskPosP <= 20) statusCheck(4);
    if (deskPosP > 20) statusCheck(3);*/
  } else if (message == "top"){
    startTime = millis();
    motorMove("up"); 
    while (((millis() - startTime) < (safeTopTime - deskPosInMilli)) && deskPosInMilli < safeTopTime) {
      stateBotS= digitalRead(botLimitSwitchS);      
      deskPosPercL("up");
    }
    motorMove("stop");
    deskPosInMilli += (millis() - startTime);
    statusCheck(3);    
  } else if (message == "bot"){
    startTime = millis();
    motorMove("down"); 
    while (((millis() - startTime) < (deskPosInMilli - safeBotTime)) && stateBotS == LOW) {
      stateBotS= digitalRead(botLimitSwitchS); 
      deskPosPercL("dn");
  }
  motorMove("stop");
  deskPosInMilli -= (millis() - startTime);
  if(stateBotS == HIGH) {  /* If switch left touching move up for 1 sec*/
    startTime = millis();                   
    motorMove("up"); 
    while (((millis() - startTime) < 1000)) {
    } 
  }
  statusCheck(4);
  } else if (message == "debounceBot"){    /* debounceBot used to lift from max bot and release endstop in case motor too slow*/
    startTime = millis();                   /* to switch from going down to up or vice versa*/
    motorMove("up"); 
    while (((millis() - startTime) < 1000)) {
      
    }
    deskPosInMilli += (millis() - startTime);
    motorMove("stop");
} else if (message == "debounceTop"){
    startTime = millis();
    motorMove("down"); 
    while (((millis() - startTime) < 1000)) {
    }
    motorMove("stop");
    deskPosInMilli -= (millis() - startTime);
    Serial.println("debounce top");
    }
}

 
