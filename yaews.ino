/*
 * yaews - Yet Another ESP Weather Station
 */
#include "BoardConfig.h"
#include "WifiConfig.h"
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <NtpClientLib.h>

#ifndef WIFI_CONFIG_H //Fallback if WifiConfig.h dose not exist (not in git repo)
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWD "YOUR_WIFI_PASSWD"
#endif // !WIFI_CONFIG_H

#define NTP_SERVER "ptbtime2.ptb.de"
int8_t timeZone = 1;
boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void setup() {
  static WiFiEventHandler e1, e2, e3;
  
  Serial.begin(115200);
  Serial.println();

  pinMode(ESP_12_LED_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
    });

  //Callback funktions for Wifi events
  e1 = WiFi.onStationModeGotIP(onSTAGotIP);
  e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
  e3 = WiFi.onStationModeConnected(onSTAConnected);
}

void loop() {
    static int i = 0;
    static int last = 0;
    
    if (syncEventTriggered) {
        processSyncEvent(ntpEvent);
        syncEventTriggered = false;
    }

    if ((millis() - last) > 5100) {
        //Serial.println(millis() - last);
        last = millis();
        Serial.print(i);
        Serial.print(" ");
        Serial.print(NTP.getTimeDateString());
        Serial.print(" ");
        Serial.print(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
        Serial.print("WiFi is ");
        Serial.print(WiFi.isConnected() ? "connected" : "not connected");
        Serial.print(". ");
        Serial.print("Uptime: ");
        Serial.print(NTP.getUptimeString());
        Serial.print(" since ");
        Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());

        i++;
    }
}

void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
    Serial.printf ("Connected to %s\r\n", ipInfo.ssid.c_str ());
}

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
  NTP.begin(NTP_SERVER, timeZone, true);
  NTP.setInterval(63);
  Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
  Serial.printf("Connected: %s\r\n", WiFi.status() == WL_CONNECTED ? "yes" : "no");
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
  Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
  Serial.printf("Reason: %d\n", event_info.reason);
  NTP.stop(); // NTP sync can be disabled to avoid sync errors
}

void processSyncEvent(NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println("Invalid NTP server address");
    } else {
        Serial.print("Got NTP time: ");
        Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
}

