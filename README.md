# yaews - Yet Another ESP Weather Station

Arduino project for the ESP8266 Mikrocontroller. The board used is the NodeMCU Amica but others should work as well.

The first LED on the ESP-12 submodule gives an indication about the wifi connection. LED off = Wifi connected
The second LED on the NodeMCU boad is currently not in use.

# Dependencies
these librarys can be installed via the arduino library manager
* [NTPClientLib](https://github.com/gmag11/NtpClient) >= 2.5.0
* [Arduino Time library](http://playground.arduino.cc/code/time) >= 1.5.0
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) >= 1.0.2
* [Adafruit DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library) >= 1.3.0
