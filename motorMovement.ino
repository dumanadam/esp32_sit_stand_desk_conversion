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
 /* Serial.println("motor Start Time is: ");
  Serial.println(startTime);
  Serial.print("stateBots is ");
  Serial.println(stateBotS);*/
  
  if(message == "stop") {
    Serial.println("motor STOPPED"); 
    motorMove("stop");
    
  } else if (message == "up"){
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going up"); 
    while (((millis() - startTime) < motorTimer) && deskPosInMilli < safeTopTime ) {
     
    }
    motorMove("stop");
    deskPosInMilli += (millis() - startTime);
    statusCheck(1);
  } else if (message == "down") {
    startTime = millis();
    motorMove("down"); 
    Serial.print("motor going down, motortimr = "); 
    Serial.println(motorTimer); 
    Serial.print("deskPosInMilli before = "); 
    Serial.println(deskPosInMilli); 
    while (((millis() - startTime) < motorTimer) && stateBotS == LOW ) {
      stateBotS= digitalRead(botLimitSwitchS);
    }
    motorMove("stop");
    deskPosInMilli -= (millis() - startTime);
    if (deskPosP <= 20) statusCheck(4);
    if (deskPosP > 20) statusCheck(3);
  } else if (message == "top"){
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going top"); 
    while (((millis() - startTime) < (safeTopTime - deskPosInMilli)) && deskPosInMilli < safeTopTime) {
      }
    motorMove("stop");
    deskPosInMilli += (millis() - startTime);
    statusCheck(3);    
  } else if (message == "bot"){
    startTime = millis();
    motorMove("down"); 
    Serial.println("motor going bot"); 
    while (((millis() - startTime) < (deskPosInMilli - safeBotTime)) && stateBotS == LOW) {
      stateBotS= digitalRead(botLimitSwitchS);      
  }
  motorMove("stop");
  deskPosInMilli -= (millis() - startTime);
  if(stateBotS == HIGH) {  /* If switch left touching move up for 1 sec*/
    startTime = millis();                   
    motorMove("up"); 
    Serial.println("motor debouncing bot"); 
    while (((millis() - startTime) < 1000)) {
    } 
  }
  statusCheck(4);
  } else if (message == "debounceBot"){    /* debounceBot used to lift from max bot and release endstop in case motor too slow*/
    startTime = millis();                   /* to switch from going down to up or vice versa*/
    motorMove("up"); 
    Serial.println("motor debouncing bot"); 
    while (((millis() - startTime) < 1000)) {
      
    }
    deskPosInMilli += (millis() - startTime);
    Serial.println("debounce bot");
    motorMove("stop");
    
} else if (message == "debounceTop"){
    startTime = millis();
    motorMove("down"); 
    Serial.println("motor debouncing Top"); 
    while (((millis() - startTime) < 1000)) {
      
    }
    motorMove("stop");
    deskPosInMilli -= (millis() - startTime);
    Serial.println("debounce top");
    
  }  else {
    Serial.println("messager error - motor stopped"); 
    motorMove("stop"); 
    /*statusCheck(1);   */
  }
}


