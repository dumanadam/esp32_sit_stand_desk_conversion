#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "credentials.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

                              /*-----------------------------------------------------------------------------------
                              
                                Change below values according to your needs, shouldnt need to change anything else

                              -------------------------------------------------------------------------------------*/


#define EEPROM_SIZE 64                        /* EEPROM size put aside for ESP32, adjust if you add any additional EEPROM storage paramaters*/ 

int maxMotorTime = 32000;                     /* Maually set  max bot to top cycle time for safety. Set as millis, 1000 = 1 sec*/
int deskTimer = 0;                            /* MQTT incoming time */
char magicChar[2] = "Q";                      /* magic charachter to check if EEPROM numbers have been stored before last shutdown, can be change to any character of your choice*/
int safeTopTime = (int)maxMotorTime *.9;      /* Set a buffer from the top - default 10%*/
int safeBotTime = (int)maxMotorTime *.1;      /* Set a buffer from the bot  - default 10%*/
const byte motor1Pin1 = 16;                   /* Motor Controller Pin 1*/
const byte motor1Pin2 = 17;                   /* Motor Controller Pin 2*/
const byte motor1Pin3 = 5;                    /* Motor Controller Pin 3*/
const byte motor1Pin4 = 18;                   /* Motor Controller Pin 4*/

const byte botLimitSwitchS = 19;              /* Bot End Stop Pin*/ 

const char* outStatus = "/Desk/status";       /* change the MQTT topic for updating the status Topic which is 3 for top and 4 for bot, 1\2 up\down */
const char* outPerc = "/Desk/perc";           /* change the MQTT topic for updating the status percentage */
const char* outDebug = "/Desk/debug";         /* change the MQTT topic for updating the debug information */
const char* inTopic = "/Desk/cmd";            /* change the MQTT topic for updating the commands to the desk through MQTT*/

const int eepMagicChar = 0, eepDeskStatus = 10, eepCycleTime = 20, eepDeskPosinMilli = 30, eepDeskPosP = 40; /* Eeprom memory storage locations */
                                              
                                              /* To be implemented , top endstop, button controls */
/*stateTopS = HIGH*/
const byte buttonPinUp = 4; 
const int buttonPinDown = 15; 
/*byte buttonStateUp = 0;   
byte buttonStateDown = 0
*/
                              /*------------------------------------------------------------------------*/

char deskStatus[2] = "8", storedMagic[2], outPayload[50], debugPayload[100], mqttIn[8];
byte stateBotS = HIGH, botLimitSwitchSError = 0, deskDirection = 0, errorStatus = 0;
int startTime, cycleTime, deskPosInMilli, deskPosP, percTarget;
const char* broker = brokerIP;

 void movePerc(int recPerc);

WiFiClient espClient;
PubSubClient client(espClient);

/* eepStatusSave updates the EEPROM and outDebug Topic on MQTT */

void eepStatusSave (char sr = 'r', char message[30] = "Happy Days", char magic[2] = magicChar, char Status[2] = deskStatus, int cycTime = cycleTime, int PosInMilli = deskPosInMilli, int Perc = deskPosP) {
  EEPROM.begin(EEPROM_SIZE);
  
  if(sr == 's'){
    EEPROM.put(eepMagicChar, magic[0]);
    EEPROM.put(eepDeskStatus,Status[0]);
    EEPROM.put(eepCycleTime, cycTime);
    EEPROM.put(eepDeskPosinMilli ,PosInMilli);
    EEPROM.put(eepDeskPosP,Perc);
    client.publish(outStatus, deskStatus);
    sprintf(outPayload, "%d", deskPosP);
    client.publish(outPerc, outPayload);
    sprintf(debugPayload, "Magic:%c deskStatus:%c cycleTime:%d deskPosinMilli:%d deskPosP: %d Error Message: %s",storedMagic[0], Status[0], cycTime, PosInMilli, Perc, message );
    Serial.println(debugPayload);
    client.publish(outDebug, debugPayload);
  } else {
  Serial.print("else in save");  
  EEPROM.get(eepMagicChar, storedMagic);
  EEPROM.get(eepDeskStatus, deskStatus[0]);
  EEPROM.get(eepCycleTime, cycleTime);
  EEPROM.get(eepDeskPosinMilli, deskPosInMilli);
  EEPROM.get(eepDeskPosP, deskPosP);
  sprintf(debugPayload, "Magic:%c deskStatus:%c cycleTime:%d deskPosinMilli:%d deskPosP: %d Error Message:\" %s\"",storedMagic[0], Status[0], cycTime, PosInMilli, Perc, message );
  client.publish(outDebug, debugPayload);
  client.publish(outStatus, deskStatus);
  sprintf(outPayload, "%d", deskPosP);
  client.publish(outPerc, outPayload);
  } /*else {
  Serial.println("  FUCK ---- eepStatsSave not s or r");
  }*/
  EEPROM.end();
 
}



void setup(){
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
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
  /*Serial.print("first read");
  eepStatusSave('r'); */
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



