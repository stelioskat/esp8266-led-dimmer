#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID   "FRITZ!Box 7272"
#define WLAN_PASS   "Aezakmi123!"

#define SERVER      "minibian"
#define SERVERPORT  1883

#define LED         4
#define DS18B20     5 // Dallas DS18B20 digital thermometer

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS18B20);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature temp_sensor(&oneWire);
float temperature;
unsigned long last_temp_millis = 0;


unsigned int i = 0;
unsigned int brightness = 1023;
bool led_on = false;
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);
Adafruit_MQTT_Subscribe onoffsw = Adafruit_MQTT_Subscribe(&mqtt, "/light/1");
Adafruit_MQTT_Publish temp_mqtt = Adafruit_MQTT_Publish(&mqtt, "/temp/1");

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();



void setup() {
  // Enable temperature sensor
  temp_sensor.begin();

  pinMode(LED, OUTPUT);
  Serial.begin(74880);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffsw);
  updateTemperature();
}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10000))) {

    if (strcmp((char *)onoffsw.lastread, "{\"On\":1}") == 0 && !led_on) {

      led_on = true;
      Serial.println(F("On"));
      i = 0;
      while (i < brightness) {
        analogWrite(LED, i);
        i += 2;
        delay(4);
      }

    }
    else if (strcmp((char *)onoffsw.lastread, "{\"On\":0}") == 0 && led_on) {

      led_on = false;
      Serial.println(F("Off"));
      while (i > 10) {
        analogWrite(LED, i);
        i -= 10;
        delay(20);
      }
      analogWrite(LED, 0);
      i = 0;

    }
    // Check if the brightness setting has been updated
    else if (strncmp((char *)onoffsw.lastread, "{\"Brightness\":", 14) == 0 ) {

      Serial.print(F("Brightness: "));
      unsigned int l = strlen((char *)onoffsw.lastread);
      onoffsw.lastread[l - 1] = '\0'; // overwrite the ending bracket
      unsigned int bright_perc = atoi((char *)onoffsw.lastread + 14);
      Serial.println(bright_perc);
      unsigned int new_brightness = 1023 * bright_perc / 100;
      if (new_brightness > 1023) new_brightness = 1023;
      if (led_on) {
        if (new_brightness > i) {
          while (i < new_brightness) {
            analogWrite(LED, i);
            i += 2;
            delay(4);
          }
          analogWrite(LED, new_brightness);
          i = new_brightness;
        }
        else {
          while (i > new_brightness) {
            analogWrite(LED, i);
            i -= 1;
            delay(2);
          }
          analogWrite(LED, new_brightness);
          i = new_brightness;
        }
      }

      brightness = new_brightness;

    }
  }

  if (millis() - last_temp_millis > 5000) {
    updateTemperature();
  }
}


void updateTemperature() {
  Serial.print("Requesting temperature...");

  temp_sensor.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature is: ");
  Serial.println(temp_sensor.getTempCByIndex(0));

  temperature = temp_sensor.getTempCByIndex(0);

  temp_mqtt.publish(temperature);

  last_temp_millis = millis();
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println(F("Retrying MQTT connection in 5 seconds..."));
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println(F("MQTT Connected!"));
}
