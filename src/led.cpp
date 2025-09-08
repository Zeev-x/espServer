#include "reyette.h"

void ledSetup(int OPIN){
    pinMode(OPIN, OUTPUT);
}

void ledControl(int OPIN, int state, int delayTime){
    for (int i = 0; i < state; i++){
        digitalWrite(OPIN, HIGH);
        delay(delayTime);
        digitalWrite(OPIN, LOW);
        delay(delayTime);
    }
}