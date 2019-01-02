void motorMove (char* motorMove) {
  if(motorMove == "stop") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, LOW); 
  } else if (motorMove == "up"){
    Serial.println("motor going up"); 
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

byte motor(char* message, unsigned long sentMotorTimer) {
  stateBotS= digitalRead(botLimitSwitchS);
  startTime = millis();
  Serial.println("motor Start Time is: ");
  Serial.println(startTime);
  Serial.print("stateBots is ");
  Serial.println(stateBotS);
  
  if(message == "stop") {
    Serial.println("motor STOPPED"); 
    motorMove("stop");
  } else if (message == "up"){
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going up"); 
    Serial.print("stateBots is ");
    while (((millis() - startTime) < maxMotorTime) ) {
      stateBotS= digitalRead(botLimitSwitchS);
      Serial.println(stateBotS);
    }
    motorMove("stop");
  } else if (message == "down") {
    startTime = millis();
    motorMove("down"); 
    Serial.println("motor going downbot"); 
    Serial.print("stateBots is ");
    while (((millis() - startTime) < maxMotorTime) && stateBotS == LOW ) {
      stateBotS= digitalRead(botLimitSwitchS);
      Serial.println(stateBotS);
    }
    motorMove("stop");
  } else if (message == "top"){
    startTime = millis();
    motorMove("up"); 
    Serial.println("motor going top"); 
    Serial.print("stateBots is ");
    while (((millis() - startTime) < maxMotorTime)) {
      stateBotS= digitalRead(botLimitSwitchS);
      Serial.println(stateBotS);
    }
    motorMove("stop");
  } else if (message == "bot"){
    startTime = millis();
    motorMove("down"); 
    Serial.println("motor going bot"); 
    Serial.print("stateBots is ");
    while (((millis() - startTime) < maxMotorTime) && stateBotS == LOW) {
    stateBotS= digitalRead(botLimitSwitchS);
    Serial.println(stateBotS);
    
    }
    motorMove("stop");
  } else {
    Serial.println("messager error - motor stopped"); 
    motorMove("stop"); 
    statusCheck(1);   
  }
 
  }
