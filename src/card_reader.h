#ifndef CARD_READER_H
#define CARD_READER_H

#include "reyette.h"

String getContentType(String filename);
bool handleFileRead(AsyncWebServerRequest *request, String path);
void initSDCard();

#endif
