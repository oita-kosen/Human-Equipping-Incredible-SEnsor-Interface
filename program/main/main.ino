#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

#include "communication.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);

  Serial.println("hello world!");
  setupCommunication();
  
  Serial.println("client setup completed");

}

void loop() {
  communication_loop();

  sendMessage("MQTT");
  
  delay(1000);
}
