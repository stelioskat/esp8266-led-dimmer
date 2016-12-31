# ESP8266 Dimmer (MQTT controlled)
This is a very simple wifi enabled (ESP8266 12e) LED dimmer for single colored LED strips.

Its purpose was originally to be controlled by Node Red (http://nodered.org) and node-red-contrib-homekit (https://flows.nodered.org/node/node-red-contrib-homekit) using MQTT events. Thus, one could control this with Apple Homekit.

Of course using MQTT you can control it with whatever you like and not necessarily with Apple Homekit.

<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/toplayer.jpg?raw=true" width="300">
<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/bottomlayer.jpg?raw=true" width="300">

## Layout

The layout is made with KiCad but I have also included an screenshot of the schematic, in case you want to make your own PCB with another software.

## Firmware

The firmware can be compiled in Arduino IDE using the available ESP8266 package. In order to flash the firmware you will need a USB-to-Serial adapter such as the FTDI232 that I am using (https://github.com/jandelgado/arduino/wiki/FTDI232-USB-to-Serial-converter).



