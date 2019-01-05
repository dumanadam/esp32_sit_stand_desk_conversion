#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "credentials.h"

/* const char* brokerUser = "gengisville";
 *  const char* brokerPass = "gengisville";
 */
#define EEPROM_SIZE 1024

 
const char* broker = "192.168.20.200";
/*const char* outTopic = "/Desk/out";*/
const char* outStatus = "/Desk/status";
const char* inTopic = "/Desk/in/cmd";
char deskStatus[2] = "8";

const byte buttonPinUp = 4; 
const byte buttonPinDown = 15; 
byte buttonStateUp = 0;   
byte buttonStateDown = 0; 
 
const byte motor1Pin1 = 16; 
const byte motor1Pin2 = 17; 
const byte motor1Pin3 = 5; 
const byte motor1Pin4 = 18;  

const byte botLimitSwitchS = 19; 
char mqttIn[8];
char mqttOut[8];
byte stateBotS = HIGH, lastBotS = HIGH, stateTopS = HIGH , botLimitSwitchSError = 0;
char messages[50];
byte deskDirection = 0, errorStatus = 0, maxDeskTime = 10 ;
int deskTimer = 0, count = 0, timeStore,debugCyc;
int maxDeskTimer = maxDeskTime * 1000;
int currentTime, startTime, endTime, cycleTime;
int maxMotorTime = 2000;
byte  looper = 1;
long diff;
char magicChar[2] = "X";


WiFiClient espClient;
PubSubClient client(espClient);

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
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP); 
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1Pin3, OUTPUT);
  pinMode(motor1Pin4, OUTPUT);
  pinMode(botLimitSwitchS, INPUT);
   client.setServer(broker, 1883);
  client.setCallback(callback);
  /*setupWifi();*/
  int temp;
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, magicChar);
  EEPROM.get(10, deskStatus);
  EEPROM.get(20, temp);
  Serial.print("magicChar  from mem 0 is");
  Serial.println(magicChar);
  Serial.print("Deskstatus from mem 10 is");
  Serial.println(deskStatus);
  Serial.print("cycleTime  from mem 20 is");
  Serial.println(cycleTime); 
  reconnect();
  setupDesk();
 

}

void loop() {
  ArduinoOTA.handle();
  yield();
  // Your programme loop from here
  client.loop();
  if ((!client.connected()) && (errorStatus!=1)){
    reconnect();
  }
}

