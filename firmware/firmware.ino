#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID   "YOUR SSID"
#define WLAN_PASS   "YOUR PASSWORD"

#define SERVER      "minibian" // The hostname of the server where the MQTT broker is installed
#define SERVERPORT  1883 // The port of the MQTT broker

#define LED         4   // Pin controlling to the LED strip.


unsigned int i = 0;
unsigned int brightness = 1023;
bool led_on = false;

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);
Adafruit_MQTT_Subscribe onoffsw = Adafruit_MQTT_Subscribe(&mqtt, "/light/1");
Adafruit_MQTT_Publish status_mqtt = Adafruit_MQTT_Publish(&mqtt, "/light/1/status");

void setup() {
  // Initialization
  pinMode(LED, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffsw);

  MQTT_connect();
  status_mqtt.publish("{\"On\":0, \"Brightness\":100}");
}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  while (mqtt.readSubscription(1000)) {

    // Turn the strip on
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
    // Turn the strip off
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
    // Update the brightness value
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
