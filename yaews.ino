/*
 * yaews - Yet Another ESP Weather Station
 */
#include "BoardConfig.h"
#include "NetworkConfig.h"
#include <Wire.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NtpClientLib.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

int8_t timeZone = 1;
boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

DHT_Unified dht(DHTPIN, DHTTYPE);

HTTPClient http;

static float validTemperatur = 0.0f;
static float validHumidity = 0.0f;

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
}

void loop() {
  static unsigned long lastSampleTime = 0 - SAMPLE_INTERVAL;
  unsigned long now = millis();
  boolean readingValid = true;

  if (syncEventTriggered) {
    processSyncEvent(ntpEvent);
    syncEventTriggered = false;
  }

  if (now - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = now;
    Serial.print(NTP.getTimeDateString());
    Serial.print(NTP.isSummerTime() ? " Summer Time. " : " Winter Time. ");

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    Serial.print("DHT22 Temperature: ");
    if (isnan(event.temperature)) {
      Serial.print("nan °C");
      readingValid = false;
    } else {
      validTemperatur = event.temperature;
      Serial.printf("%.2f °C", validTemperatur);
    }

    dht.humidity().getEvent(&event);
    Serial.print(" Humidity: ");
    if (isnan(event.relative_humidity)) {
      Serial.print("nan %\r\n");
      readingValid = false;
    } else {
      validHumidity = event.relative_humidity;
      Serial.printf("%.2f %\r\n", validHumidity);
    }

    if (readingValid == true) {
      if((WiFi.status() == WL_CONNECTED)){
        sendValues();
      }else{
        Serial.println("Error in WiFi connection");
      }
    } else {
      Serial.println("One or more sensor reading invalid");
    }
  }
}

void sendValues() {
  digitalWrite(LED_BUILTIN, LOW);
  String influxData = "";
  char strBuffer[10];
  int httpCode = -1;

  // Set data type and add tags
  influxData += "air_temperature,house=test,position=balcony value=";
  influxData += dtostrf(validTemperatur, 5, 2, strBuffer);
  influxData += "\n";
  influxData += "air_humidity,house=test,position=balcony value=";
  influxData += dtostrf(validHumidity, 5, 2, strBuffer);
  influxData += "\n";
  // Serial.println(influxData);

  http.begin(DB_SERVER, DB_PORT, DB_DATABASE_URI);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setAuthorization(DB_USER, DB_PASSWD);
  http.setTimeout(2000);

  while(httpCode == -1){
    httpCode = http.POST(influxData);
    http.writeToStream(&Serial);
  }
  http.end();
  digitalWrite(LED_BUILTIN, HIGH);
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
