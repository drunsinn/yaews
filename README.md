# yaews - Yet Another ESP Weather Station

Arduino project for the ESP8266 Mikrocontroller. The board used is the NodeMCU Amica but others should work as well.

The first LED on the ESP-12 submodule gives an indication about the wifi connection. LED off = Wifi connected
The second LED on the NodeMCU boad is currently not in use.

# Hardware

## BME 280
This sensor is connected via the i2c bus.
busline SCL is on D1
busline SDA is on D2
Change the slave address in BoardConfig.h to the configuration of the SDO pin.

# Dependencies

These librarys can be installed via the arduino library manager:
* [NTPClientLib](https://github.com/gmag11/NtpClient) >= 2.5.0
* [Time](http://playground.arduino.cc/code/time) >= 1.5.0 (search for `timekeeping` in library manager)
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) >= 1.0.2
* [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library) >= 1.0.7

    ```bash
cd ~/Arduino/libraries
git clone https://github.com/hwwong/ESP_influxdb.git
```

# Known Issues

## Watchdog reset on boot
Depending on wich typ of esp8266 module you use, you have to change the build options in the arduino ide.
For me, i had to changed the Flash Size to "4M (3SPIFFS)" or the module would not boot
