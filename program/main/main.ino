#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsClient.h>

// credentials.hは付属の`credentials_sample.h`に従って
// 自分で用意してください。
#include "credentials.h"

WebSocketsClient webSocket;

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

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);

			          // send message to server when Connected
				        webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

			    // send message to server
			    // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
		case WStype_ERROR:
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(23, OUTPUT);
  delay(500);

  connectWiFi();

  webSocket.beginSSL("shielded-sea-64404.herokuapp.com", 81, "/echo");
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000);
  //connectWiFi();
}

void loop() {
  static int count;

  String str(count);
  webSocket.sendTXT(str);
  //Serial.println(str);
  count++;
  delay(1000);

  webSocket.loop();
}
