#include "reyette.h"
#include "webHandler.h"

AsyncWebServer server(80);

bool handleFileRead(AsyncWebServerRequest *request, String path);

void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Hello, world!");
}

void webServerSetup() {
  server.onNotFound([](AsyncWebServerRequest *request) {
    String path = request->url();
    if (!handleFileRead(request, path)) {
      request->send(404, "text/plain", "File tidak ditemukan: " + path);
    }
  });

  server.begin();
}
