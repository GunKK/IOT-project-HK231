#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"
#include <DHTesp.h>

#define DHT_PIN 4

DHTesp dht;

int fan = 20;

unsigned long previousMillis = millis();

void sendMQTTvalues(float temp, float hum)
{
  StaticJsonDocument<256> doc;

  doc["device"] = "ESP32";
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  char buff[256];
  serializeJson(doc, buff);
  client.publish("ESPValues",buff);
}

void setup() {
  Serial.begin(115200);
  // DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);
  // led RGB
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);

  connectAP();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop()) {
    client.connect("ESP32-");
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000) {
    previousMillis = currentMillis;

    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    if (!isnan(temperature) && !isnan(humidity)) {
      sendMQTTvalues(temperature, humidity);
    }
    client.publish("toNodeRED", "hello from ESP32");
  }
}