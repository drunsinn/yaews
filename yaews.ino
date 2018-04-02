/*
 * yaews - Yet Another ESP Weather Station
 */
#include "BoardConfig.h"
#include "WifiConfig.h"
#include "DatabaseConfig.h"
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <NtpClientLib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#ifndef WIFI_CONFIG_H //Fallback if WifiConfig.h does not exist (not in git repo)
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWD "YOUR_WIFI_PASSWD"
#endif // !WIFI_CONFIG_H

#ifndef DATABASE_CONFIG_H //Fallback if DatabaseConfig.h does not exist (not in git repo)
#define DB_SERVER "example.com"
#define DB_PORT 8086
#define DB_USER "user"
#define DB_PASSWD "passwd"
#define DB_DATABASE "db"
#endif //!DATABASE_CONFIG_H

#define NTP_SERVER "ptbtime2.ptb.de"
int8_t timeZone = 1;
boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  static WiFiEventHandler e1, e2, e3;

  Serial.begin(115200);
  Serial.println();

  // both LED are low-active
  pinMode(ESP_12_LED_PIN, OUTPUT);
  digitalWrite(ESP_12_LED_PIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
    });

  // Callback functions for Wifi events
  e1 = WiFi.onStationModeGotIP(onSTAGotIP);
  e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
  e3 = WiFi.onStationModeConnected(onSTAConnected);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
}

void loop() {
    static int i = 0;
    static int last = 0;

    if (syncEventTriggered) {
        processSyncEvent(ntpEvent);
        syncEventTriggered = false;
    }

    if ((millis() - last) > 5100) {
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
        Serial.print(NTP.getTimeDateString(NTP.getFirstSync()).c_str());

        sensors_event_t event;
        // Get temperature event and print its value.
        dht.temperature().getEvent(&event);
        if (isnan(event.temperature)) {
          Serial.println("Error reading temperature!");
        } else {
          Serial.print(" Temperature: ");
          Serial.print(event.temperature);
          Serial.print(" *C");
        }
        // Get humidity event and print its value.
        dht.humidity().getEvent(&event);
        if (isnan(event.relative_humidity)) {
          Serial.println("Error reading humidity!");
        } else {
          Serial.print(" Humidity: ");
          Serial.print(event.relative_humidity);
          Serial.println("%");
        }
        i++;
    }
}

// Callback for successfull connection to Wifi
void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
  Serial.printf ("Connected to %s\r\n", ipInfo.ssid.c_str ());
  digitalWrite(ESP_12_LED_PIN, HIGH);
}

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
  Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
  Serial.printf("Connected: %s\r\n", WiFi.status() == WL_CONNECTED ? "yes" : "no");

  NTP.begin(NTP_SERVER, timeZone, true);
  NTP.setInterval(63);
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
  Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
  Serial.printf("Reason: %d\n", event_info.reason);
  digitalWrite(ESP_12_LED_PIN, LOW);
  NTP.stop();
}

// Callback for NTP connection
void processSyncEvent(NTPSyncEvent_t ntpEvent) {
  if (ntpEvent) {
    Serial.print("Time Sync error: ");
    if (ntpEvent == noResponse){
      Serial.println("NTP server not reachable");
    } else if (ntpEvent == invalidAddress){
      Serial.println("Invalid NTP server address");
    }
  } else {
    Serial.print("Got NTP time: ");
    Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
  }
}
