# Convert a sit\stand desk to an ESP32 MQTT controlled motorised desk

A project to convert a manually "Cranked" sit stand desk into one driven by a motor and can be controlled through MQTT commands for use through mobile apps or home automation.

For hardware used, wiring and more information see [here](https://talha.me/)

## Installation

Use the Arduino IDE to open the ino file. Upload to the ESP32 witrh the settings below.

![ESP32 Settings](ide.jpg)


## Usage
When the system boots on for the first time it will check if there is a match on the magic number. If not it will go through its boot cycle 


* Log on to wireless
* Log onto MQTT server
* Read EEPROM for magic number
* Broadcast last position or cycle to check motor and start in "Top" position

The only changes that should be required are the variables marked in "desk.ino" with comments.


```C++
define EEPROM_SIZE 64                        /* EEPROM size put aside for ESP32, adjust if you add any additional EEPROM storage paramaters*/ 

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
```

## Contributing
Pull requests are welcome.


## License
[GNU](https://www.gnu.org/licenses/gpl-3.0.en.html)
