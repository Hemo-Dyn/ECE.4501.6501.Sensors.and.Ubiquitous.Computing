#include "FastPD.hpp"
#include "Adafruit_TLC59711.h"


#define NUM_TLC59711 1
#define DATA_PIN  10
#define CLOCK_PIN 8
Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, CLOCK_PIN, DATA_PIN);
FastPD* fast;

void allLedsOff() {
    tlc.begin();
    for (int i = 0; i < LED_NUM; i++) {
        tlc.setPWM(i, 0);
    }
    tlc.write();
}

void setup() {
    Serial.begin(9600);
    Serial.println("beginning init");
    fast = new FastPD();
    Wire.begin();
        allLedsOff();
        if (!fast->begin()) {
            Serial.println("Could not find AS7341");
            while (1) { delay(1000); }
        }
        tlc.begin();
        delay(1000);
    Serial.println("Init done");
}

void loop() {
    Serial.println("hello, man");
    delay(500);
}
