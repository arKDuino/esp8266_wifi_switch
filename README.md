
# esp8266 relay control for lamps
A simple relay control for lamps, ovens, air conditioners, everything that plugs in a wall and use electricity. Using Arduino IDE.

* Just access your device's IP (or mDNS address) to turn on/off;
* Only accessible inside your LAN;
* Internal HTTP web server, access without app.
* PCB included (made on easyEda);
* Internally interfaces with PCF8574 input/output I2C module, which is extremely stable;
* Auto-restart when something goes wrong;
* MDNS support (access via http://name.local);
* All language specific strings are in constants_lang_pt.h

Just write the .ino sketch into esp8266, turn on, press the button and connect to configuration AP.

PS: Don't forget to power the module while programming, and connect your FTDI/Arduino/programmer GND to board's GND.

PS2: New ESP8266 chips eventually comes with rubbish data inside EEPROM, upload the sketch/firmware with
```
#define ERASE_EEPROM_NEW_CHIP true
```
Run the code, remove this #define and upload the code again. 

Board mounted on PCB: (from https://jlcpcb.com/)

![Final Board](https://raw.githubusercontent.com/gustavofbreunig/esp8266_wifi_switch/master/complete_board.jpg)



### TODO:
 -  demonstration gif
