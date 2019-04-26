#include <Arduino.h>
#include <WiFi.h>

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// credentials.hは付属の`credentials_sample.h`に従って
// 自分で用意してください。
#include "credentials.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

const char* mqtt_topic_lookback = "device/sensor";
/**
 * Connect WiFi
 */
void connectWiFi()
{
    WiFi.begin(ssid, wifi_pass);
    Serial.print("WiFi connecting...");
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.print(" connected. ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("[");
  Serial.print(topic);
  Serial.print("] ");
  char temp[64];
  int lim = (length < 64 ? length : 63);
  int i;
  for (i = 0; i < lim; i++) {
    temp[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  temp[i] = '\0';
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("==================");
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println("==================");
      Serial.println("");
      client.subscribe("device/sensor");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void communication_loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setupMQTT()
{
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void sendMessage(const char* str)
{
  client.publish(mqtt_topic_lookback, str);
}

void setupCommunication()
{
  connectWiFi();
  setupMQTT();
}