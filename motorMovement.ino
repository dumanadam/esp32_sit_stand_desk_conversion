
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




void motor(char message[]/*, int motorTimer = maxMotorTime*/) {
  
  stateBotS= digitalRead(botLimitSwitchS);
  startTime = millis();
  if(message == "stop") {
    motorMove("stop");
  } else if (message == "up"){
    startTime = millis();
    motorMove("up"); 
    Serial.print("motor up percTarget = " );
    Serial.println(percTarget);
    movePerc(percTarget);
    motorMove("stop");
    deskPosInMilli += (millis() - startTime);
    /*statusCheck(1);*/
    
  } else if (message == "down") {
    startTime = millis();
    motorMove("down"); 
    Serial.print("motor down percTarget = " );
    Serial.println(percTarget);
    movePerc(percTarget);
    motorMove("stop");
    deskPosInMilli -= (millis() - startTime);
    /*statusCheck(2);*/
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

 
