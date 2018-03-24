 #pragma once
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define ESP_12_LED_PIN 2

#define HOSTNAME "yaews"

/*
 * D0  = GPIO16 = LED 2
 * D1  = GPIO5  = SCL
 * D2  = GPIO4  = SDA
 * D3  = GPIO0 
 * D4  = GPIO2  = TX D1 = LED 1
 * D5  = GPIO14 = SPI SCK
 * D6  = GPIO12 = SPI MISO
 * D7  = GPIO13 = SPI MOSI = RX D2
 * D8  = GPIO15 = SPI SS   = TX D2
 * D9  = GPIO3  = RX DO
 * D10 = GPIO1  = TX DO
 * 
 * taken from nodemcu/pins_arduino.h
 * #define PIN_WIRE_SDA (4)
 * #define PIN_WIRE_SCL (5)
 * #define LED_BUILTIN 16
 * 
 * taken from generic/common.h
 * #define PIN_SPI_SS   (15)
 * #define PIN_SPI_MOSI (13)
 * #define PIN_SPI_MISO (12)
 * #define PIN_SPI_SCK (14)
 */

#endif //BOARD_CONFIG_H
