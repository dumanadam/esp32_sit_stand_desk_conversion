#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "credentials.h"

/* const char* brokerUser = "gengisville";
 *  const char* brokerPass = "gengisville";
 */
const char* broker = "192.168.20.200";
const char* outTopic = "/Desk/out";
const char* outStatus = "/Desk/status";
const char* inTopic = "/Desk/cmd";
char* deskStatus = "3";

const byte buttonPinUp = 4; 
const byte buttonPinDown = 15; 
byte buttonStateUp = 0;   
byte buttonStateDown = 0; 
 
const byte motor1Pin1 = 16; 
const byte motor1Pin2 = 17; 
const byte motor1Pin3 = 5; 
const byte motor1Pin4 = 18;  
const byte LED_BUILTIN = 2;

const byte botLimitSwitchS = 19; 
byte botLimitSwitchSError = 0;



byte stateBotS = HIGH;
byte lastBotS = HIGH;
byte stateTopS = HIGH;
int count = 0;
char messages[50];
byte deskDirection = 0;
int deskTimer = 0; 
byte maxDeskTime = 10;
int maxDeskTimer = maxDeskTime * 1000;
unsigned long currentTime, startTime, cycleTime, endTime;
unsigned long maxMotorTime = 10000;
unsigned long topMotorTime = 10000;
unsigned long botMotorTime = 10000;
byte  looper = 1;
long diff;
byte errorStatus = 0;

byte motor(char* motorMove, unsigned long motorTimer = 0);


WiFiClient espClient;
PubSubClient client(espClient);

/*void setupWifi() {
  delay(100);
  Serial.println("\nConnecting to");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("connecting....");
  }

  Serial.print("\nConnected to " );
  Serial.println(ssid);
  
}
*/

void setup(){
  // Setup needed for OTA
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Port defaults to 3232                            // ArduinoOTA.setPort(3232);
  // Hostname defaults to esp3232-[MAC]               // ArduinoOTA.setHostname("myesp32");
  // No authentication by default                     // ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)         Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 //  My setup onwards
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP); 
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1Pin3, OUTPUT);
  pinMode(motor1Pin4, OUTPUT);
  pinMode(botLimitSwitchS, INPUT);
  pinMode (LED_BUILTIN, OUTPUT);
  /*setupWifi();*/
  setupDesk();
  client.setServer(broker, 1883);
  client.setCallback(callback);
  reconnect();
  statusCheck(4);

}

void loop() {
  ArduinoOTA.handle();
  yield();
  // Your programme loop from here
  client.loop();
  if ((!client.connected()) && (errorStatus!=1)){
    reconnect();
  }
 /* stateBotS= digitalRead(botLimitSwitchS);
  if (stateBotS == LOW) {
    motor("stop");
    Serial.println("Switch S = clicked"); 
  } 
*/
/*  buttonStateUp = digitalRead(buttonPinUp);
  buttonStateDown = digitalRead(buttonPinDown);
  


  if (buttonStateUp == LOW) {
    Serial.println("buttonstateup = LOW");
    Serial.println("Moving Forward");
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, HIGH); 
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(2000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    Serial.println("Motor stopped");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, LOW); 
    }
    
  if (buttonStateDown == LOW) {
    Serial.println("buttonstatedown = LOW");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
     // Move DC motor backwards at maximum speed
    Serial.println("Moving Backwards");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH); 
    digitalWrite(motor1Pin3, HIGH);
    digitalWrite(motor1Pin4, LOW); 
    delay(2000);
  
    // Stop the DC motor
    Serial.println("Motor stopped");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 
    digitalWrite(motor1Pin3, LOW);
    digitalWrite(motor1Pin4, LOW); 
    } */
      
 /* currentTime = millis();
  if(currentTime - lastTime > 2000) {
    count++;
    snprintf(messages, 75, "Count:%ld", count);
    Serial.print("Sending messages");
    Serial.println(messages);
    client.publish(outTopic, messages);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    lastTime = millis ();
   
  }*/
  // End of your programme loop
}
