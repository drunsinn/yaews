# yaews - Yet Another ESP Weather Station

Arduino project for the ESP8266 Mikrocontroller. The board used is the NodeMCU Amica but others should work as well.

The first LED on the ESP-12 submodule gives an indication about the wifi connection. LED off = Wifi connected
The second LED on the NodeMCU boad is currently not in use.

# Hardware

## DHT22/AM2302
The DAT-Pin of the DHT22 sensor is connected to GPIO0. This corresponds to D3 on the NodeMCU Amica board.

# Dependencies

These librarys can be installed via the arduino library manager:
* [NTPClientLib](https://github.com/gmag11/NtpClient) >= 2.5.0
* [Time](http://playground.arduino.cc/code/time) >= 1.5.0 (search for `timekeeping` in library manager)
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) >= 1.0.2
* [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) >= 1.3.0

These librarys currently have to be installed manually:
* [ESP_influxdb](https://github.com/hwwong/ESP_influxdb)

    ```bash
cd ~/Arduino/libraries
git clone https://github.com/hwwong/ESP_influxdb.git
```
