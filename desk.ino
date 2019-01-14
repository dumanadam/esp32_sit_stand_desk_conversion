#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "credentials.h"

/* const char* brokerUser = "";
 *  const char* brokerPass = "";
 */
#define EEPROM_SIZE 64
; 

int maxMotorTime = 5500;                     /* Maually set  max bot to top cycle time for safet. Set as millis.*/
int deskTimer = 0;                            /* MQTT incoming time */
char magicChar[2] = "V", storedMagic[2];                      /* magic number to check if EEPROM numbers have been stored*/
int safeTopTime = (int)maxMotorTime *.9;      /* Set to reverse after hitting stop - default 10%*/
int safeBotTime = (int)maxMotorTime *.1;      /* Set to reverse after hitting stop - default 10%*/
 
const char* broker = "192.168.20.200";
const char* outStatus = "/Desk/status";
const char* outPerc = "/Desk/perc";
const char* outDebug = "/Desk/debug";
const char* inTopic = "/Desk/cmd";

                                              /* To be implemented , top endstop, button controls
stateTopS = HIGH*/
const byte buttonPinUp = 4; 
const byte buttonPinDown = 15; 
/*byte buttonStateUp = 0;   
byte buttonStateDown = 0
*/

char deskStatus[2] = "8"; 

const byte motor1Pin1 = 16;                   /* Motor Controller Pins*/
const byte motor1Pin2 = 17; 
const byte motor1Pin3 = 5; 
const byte motor1Pin4 = 18;  

const byte botLimitSwitchS = 19;              /* Bot End Stop Pin*/

char mqttIn[8];
char mqttOut[8];
byte stateBotS = HIGH, lastBotS = HIGH, botLimitSwitchSError = 0;
byte deskDirection = 0, errorStatus = 0;

int startTime, cycleTime, deskPosInMilli, deskPosP;

void eepStatusSave (char sr = 'r', char Status[2] = deskStatus, int PosInMilli = deskPosInMilli, int Perc = deskPosP, int cycTime = cycleTime) {
  EEPROM.begin(EEPROM_SIZE);
  char temp[10];
  Serial.print("sr is ");
  Serial.println(sr);
  if(sr == 's'){
    Serial.println(" eepStatsSave ---- status, PosinMilli, Perc");
    Serial.println(Status);
    Serial.println(PosInMilli);
    Serial.println(Perc);
    EEPROM.put(10,Status[0]);
    EEPROM.put(30,PosInMilli);
    EEPROM.put(40,Perc);
  } else if(sr == 'r') {
  EEPROM.get(0, storedMagic);
  EEPROM.get(10, deskStatus[0]);
  EEPROM.get(20, cycleTime);
  EEPROM.get(30, deskPosInMilli);
  EEPROM.get(40, deskPosP);
  Serial.print("magicInMem  from mem 0 is ");
  Serial.println(storedMagic);
  Serial.print("Deskstatus from mem 10 is ");
  Serial.println(deskStatus[0]);
  Serial.print("cycTime from mem 20 is ");
  Serial.println(cycleTime); 
  Serial.print("deskPosInMilli from mem 30 is ");
  Serial.println(cycleTime); 
  Serial.print("deskPosP from mem 40 is ");
  Serial.println(deskPosP); 
  } else {
  Serial.println("  ---- eepStatsSave not s or r");
  }
  EEPROM.end();
 
}


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
  EEPROM.begin(EEPROM_SIZE);
  Serial.print("first read");
  eepStatusSave('r'); 
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



