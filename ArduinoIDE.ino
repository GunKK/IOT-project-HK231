#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"
#include <DHTesp.h>

#define DHT_PIN 4
#define MQ2_A 33
#define MQ2_D 32

float D_value, A_value;

DHTesp dht;

int fan = 20;

unsigned long previousMillis = millis();

void sendMQTTvalues(float temp, float hum, float gas)
{
  StaticJsonDocument<256> doc;

  doc["device"] = "ESP32";
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["gas"] = gas;

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

  // fan
  pinMode(FAN, OUTPUT);

  // 
  pinMode(MQ2_D, INPUT);

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

  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    
    A_value=analogRead(MQ2_A);
    D_value=digitalRead(MQ2_D);

    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    if (!isnan(temperature) && !isnan(humidity)) {
      sendMQTTvalues(temperature, humidity, A_value);
    }
    client.publish("toNodeRED", "hello from ESP32");
  }
}