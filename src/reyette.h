#ifndef REYETTE_H
#define REYETTE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#if defined(esp32)
#include <WiFi.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#define FS_SYSTEM SPIFFS
#elif defined(esp8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <LittleFS.h>
#define FS_SYSTEM LittleFS
#endif

#define SD_CS   5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK  18


#define PIN 2

#define wifi_ssid_default "YOUR_SSID"
#define wifi_password_default "YOUR_PASSWORD"

const IPAddress static_ip_default(192, 168, 0, 3);
const IPAddress gateway_ip_default(192, 168, 0, 1);
const IPAddress subnet_ip_default(255, 255, 255, 0);
const IPAddress primary_dns_default(192, 168, 100, 1);
const IPAddress secondary_dns_default(8, 8, 8, 8);

#define upload_folder "/files"

#endif