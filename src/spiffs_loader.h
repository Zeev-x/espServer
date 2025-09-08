#ifndef SPIFFS_LOADER_H
#define SPIFFS_LOADER_H

#include <Arduino.h>
#include <FS.h>

#if defined(ESP32)
  #include "SPIFFS.h"
  #define FS_SYSTEM SPIFFS
#elif defined(ESP8266)
  #include <LittleFS.h>
  #define FS_SYSTEM LittleFS
#endif

void spiffsSetup();

#endif
