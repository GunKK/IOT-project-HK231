#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"


// MQTT

WiFiClient espClient;
bool useMQTT = true;
PubSubClient client(espClient);
StaticJsonDocument<200> doc;

const int PIN_RED   = 15;
const int PIN_GREEN = 2;
const int PIN_BLUE  = 0;

void setColor(int R, int G, int B) {
  analogWrite(PIN_RED,   R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE,  B);
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32", mqtt_user, mqtt_password))
        {
            Serial.println("connect to MQTT");
            client.subscribe("fromNodeRED");
            client.subscribe("JSONfromNodeRED");

            client.subscribe("Iot/rgbled");  
            Serial.println("Subscribed");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void callback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }

    // control led color
    if (String(topic) == "Iot/rgbled") {
        Serial.print("Changing Color");
        Serial.print(messageTemp);
        Serial.println();

        deserializeJson(doc, messageTemp);
        int r = doc["r"];
        int g = doc["g"];
        int b = doc["b"];
        Serial.println(r);
        Serial.println(g);
        Serial.println(b);
        setColor(r, g, b);
    }
}

void connectAP()
{
    Serial.println("Connect to my WiFi");
    WiFi.begin(ssid, password);
    byte cnt = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        cnt++;

        if (cnt > 30)
        {
            ESP.restart();
        }
    }
}