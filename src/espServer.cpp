#include "reyette.h"
#include "led.h"

void initSDCard();
void spiffsSetup();
void networking();
void uploadFile();
void webServerSetup();

void espServerSetup() {
  Serial.begin(115200);
  ledSetup(PIN);
  initSDCard();
  spiffsSetup();
  networking();
  uploadFile();
  webServerSetup();
}