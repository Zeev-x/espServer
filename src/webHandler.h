#ifndef WEBHANDLER_H
#define WEBHANDLER_H

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void handleRoot(AsyncWebServerRequest *request);
void webServerSetup();

#endif
