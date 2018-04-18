# ESP8266 LED Dimmer (MQTT controlled)

1. [Layout](#layout)
  1. [Parts](#parts)
  2. [Schematic](#schematic)
2. [Firmware](#firmware)
  1. [MQTT](#mqtt)

This is a very simple wifi enabled (ESP8266 12e) LED dimmer for single colored LED strips. 

Its purpose was originally to be controlled by Node-RED (http://nodered.org) and node-red-contrib-homekit (https://flows.nodered.org/node/node-red-contrib-homekit) using MQTT events. Thus, one could control this with Apple Homekit.

Of course by using MQTT you can control it with whatever you like and not necessarily with Apple Homekit.

<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/toplayer.jpg?raw=true" width="300">
<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/bottomlayer.jpg?raw=true" width="300">


## Layout

The layout is made with KiCad but I have also included an screenshot of the schematic, in case you want to make your own PCB with another software. You could simply produce this PCB for example by uploading the `ESPDimmer.kicad_pcb` file directly in [AISLER](https://go.aisler.net) .

<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/top.png?raw=true" width="300">
<img src="https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/bot.png?raw=true" width="300">

### Parts
<table>
<tr>
  <th>
    Nr.
  </th>
  <th>
    Part
  </th>
  <th>
    Package
  </th>
  <th>
    Qty.
   </th>
</tr>
<tr>
  <td>1</td>
  <td>Capacitor 100nF</td>
  <td>0402</td>
  <td>2</td>
</tr>
<tr>
  <td>2</td>
  <td>Capacitor 1uF</td>
  <td>Package 0402</td>
  <td>1</td>
</tr>
<tr>
  <td>3</td>
  <td>Resistor 10k</td>
  <td>Package 0805</td>
  <td>1</td>
</tr>
<tr>
  <td>4</td>
  <td>Voltage regulator TLV1117</td>
  <td>SOT-223</td>
  <td>1</td>
</tr>
<tr>
  <td>5</td>
  <td>AK300-2 Terminal for LED strip</td>
  <td>-</td>
  <td>1</td>
</tr>
<tr>
  <td>6</td>
  <td>ESP8266-12E</td>
  <td></td>
  <td>1</td>
</tr>
<tr>
  <td>7</td>
  <td>Standard barrel jack for power supply</td>
  <td></td>
  <td>1</td>
</tr>
 <tr>
  <td>8</td>
  <td>30V Single N-Channel Power MOSFET</td>
  <td>TO-220</td>
  <td>1</td>
</tr>
</table>

### Schematic

![Schematic](https://github.com/stelioskat/ESP8266-Dimmer/blob/master/pics/schematic.png?raw=true)

## Firmware

The firmware can be compiled in Arduino IDE using the available ESP8266 package. In order to flash the firmware you will need a USB-to-Serial adapter such as the FTDI232 that I am using (https://github.com/jandelgado/arduino/wiki/FTDI232-USB-to-Serial-converter). Keep in mind that you will have to update the Wifi credentials and also the MQTT broker hostname and port to fit your own.

Of course, you could write your own firmware or you use another one since this is basically a breakout board.

### MQTT

The default firmware subscribes to the topic "/light/1" and expects the following JSON messages to control the LED strip and its brightness.

- Turn on command:
```
{"On":1}
```
- Turn off command:
```
{"On":0}
```
- Set brightness level:
```
{"Brightness":<A VALUE BETWEEN 0 AND 100>}
```
