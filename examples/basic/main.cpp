#include "reyette.h"
#include "led.h"

void initSDCard();
void spiffsSetup();
void networking();
void uploadFile();
void webServerSetup();

void setup() {
  Serial.begin(115200);
  ledSetup(PIN);
  initSDCard();
  spiffsSetup();
  networking();
  uploadFile();
  webServerSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
}