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
#include <Adafruit_BME280.h>

boolean syncEventTriggered = false;
NTPSyncEvent_t ntpEvent;

HTTPClient http;

Adafruit_BME280 bme;
boolean bmeDisabled = false;

void setup() {
  static WiFiEventHandler e1, e2, e3;
 
  Serial.begin(115200);
  Serial.println();
  Serial.println();
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

  if (!bme.begin(BME280_ADDR, &Wire)) {
    bmeDisabled = true;
    Serial.println("BME 280 not found, disable reading");
  } else {
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
  }
//  http.setReuse(true);
  Serial.println("Setup finished");
}

void loop() {
  static unsigned long lastSampleTime = 0 - SAMPLE_INTERVAL;
  unsigned long now = millis();

  if (syncEventTriggered) {
    processSyncEvent(ntpEvent);
    syncEventTriggered = false;
  }

  if (now - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = now;
    
    Serial.print (NTP.getTimeDateString ());
    Serial.print (" ");
    Serial.print (NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
    Serial.print ("WiFi is ");
    Serial.print (WiFi.isConnected() ? "connected" : "not connected");
    Serial.print (". ");
    Serial.print ("Uptime: ");
    Serial.print (NTP.getUptimeString());
    Serial.print (" since ");
    Serial.println (NTP.getTimeDateString(NTP.getFirstSync()).c_str());

    if (!bmeDisabled) {
      bme.takeForcedMeasurement();
    }

    if (WiFi.isConnected()){
      updateInfluxDB();
    }
  }
}

void updateInfluxDB() {
  digitalWrite(LED_BUILTIN, LOW);
  String influxData = "";
  char strBuffer[15];
  int httpCode = -1;

  if (!bmeDisabled) {
    // Set data type and add tags
    influxData += "air_temperature,house=test,position=balcony value=";
    influxData += dtostrf(bme.readTemperature(), 5, 2, strBuffer);
    influxData += "\n";
    influxData += "air_humidity,house=test,position=balcony value=";
    influxData += dtostrf(bme.readHumidity(), 5, 2, strBuffer);
    influxData += "\n";
    influxData += "air_preassure,house=test,position=balcony value=";
    influxData += dtostrf(bme.readPressure() / 100.0f, 5, 2, strBuffer);
    influxData += "\n";
  }
  influxData += "local_time,house=test,position=balcony value=";
  influxData += itoa(now(), strBuffer, 10);
  influxData += "\n";
  influxData += "up_time,house=test,position=balcony value=";
  influxData += itoa(NTP.getUptime(), strBuffer, 10);
  influxData += "\n";

  http.setTimeout(2000);
  http.begin(DB_SERVER, DB_PORT, DB_DATABASE_WRITE_URI);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setAuthorization(DB_USER, DB_PASSWD);
  
  httpCode = http.POST(influxData);
  if (httpCode >= 400){
    Serial.println(http.getString());
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

  NTP.begin(NTP_SERVER, NTP_TIMEZONE, NTP_USE_DST);
  NTP.setInterval(NTP_RESYNC_INTERVAL);
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
  Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
  Serial.printf("Reason: %d\n", event_info.reason);
  digitalWrite(ESP_12_LED_PIN, LOW);
  NTP.stop();
}

//// Callback for NTP connection
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
