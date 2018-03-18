/*
 * yaews - Yet Another ESP Weather Station
 */
#include "BoardConfig.h"
#include "WifiConfig.h"
#include <ESP8266WiFi.h>

#ifndef WIFI_CONFIG_H //Fallback if WifiConfig.h dose not exist (not in git repo)
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWD "YOUR_WIFI_PASSWD"
#endif // !WIFI_CONFIG_H

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ESP_12_LED_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  digitalWrite(ESP_12_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(ESP_12_LED_PIN, LOW);
  delay(1000);
}
