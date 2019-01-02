int extractor(char* sentTopic, byte* sentPayload, unsigned int sentLength){

  char mqttIn[8] = "";
  int newLength = sentLength;

  Serial.println(""); 
  Serial.print("Message arrived [");
  Serial.print(sentTopic);
  Serial.println("] ");
  
  for (int i=0;i<sentLength;i++){
    mqttIn[i] = (char)sentPayload[i];
  }

  if ((mqttIn[0] < '0' or  mqttIn[1] != ',')){
    Serial.print("you shouldnty see this");
    } else {
    deskDirection = mqttIn[0]- '0';
    memset(mqttIn, 0, sizeof(mqttIn));
   /* mqttIn[newlength +1] = '\0';*/
    newLength -=2;
    for (int i=0; i<newLength; i++) {
      mqttIn[i] = (char)sentPayload[i+2];
      }  

    deskTimer = atoi(mqttIn);
    if (deskTimer > maxDeskTimer ) deskTimer = 10;

    
    Serial.println(deskDirection);
    Serial.println(deskTimer);
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  
  extractor(topic, payload, length);
  if (deskDirection == 0){
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    // Stop the DC motor
    Serial.println("Motor stopped");
    motor("stop");
 
    }
      if (deskDirection == 1){
        Serial.println("Received desk_up");
        motor("up");
        digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
        digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
        statusCheck(2);
      }
    
    if (deskDirection == 2){
      Serial.println("Received desk DOWN");
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      motor("down");
      statusCheck(2);
    }
    if (deskDirection == 3){
      Serial.println("Received desk TOP");
      motor("top");
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      statusCheck(3);
      }
    if (deskDirection == 4){
      Serial.println("Received desk Bot");
      motor("bot");
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      statusCheck(4);
      }
 
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnnecting to ");
    Serial.print(broker);
    if (client.connect("desk")){
      Serial.println("\nSuccess! Connected to broker!");
      client.subscribe(inTopic);
    } else {
      Serial.println("Trying again in 5 seconds");
      delay(5000);
      }
      }
}
