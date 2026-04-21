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

void print_debug() {
    Serial.println("hello");
    delay(500);
}

void setup() {
    Serial.begin(9600);
    Serial.println("beginning init");
    Wire.begin();
    fast = new FastPD();
    allLedsOff();
    
    if (!fast->begin()) {
        Serial.println("Could not find AS7341");
        while (1) { delay(1000); }
    }

    fast->init();
    tlc.begin();
    delay(1000);

    if (!fast->loadDynamicSMUX()) {
        Serial.println("Error: Too many diodes requested for single-pass SMUX!");
    }
    Serial.println("Init done");
}

void loop() {
    for (int i = 0; i < LED_NUM; i++) {
        // 1. Turn LED ON
        tlc.setPWM(i, 5000);
        tlc.write();
        
        // 2. Wait for Integration + LED Settle time
        // Total wait must be > 71ms for your current ATIME/ASTEP settings
        delay(80); 

        // 3. Read the now-stable spectral data
        fast->update_readings();

        // 4. Print results
        Serial.print("LED ["); Serial.print(i); Serial.print("] Data: ");
        for (int p = 0; p < 10; p++) {
            Serial.print(fast->readings[p]);
            Serial.print(",");
        }
        Serial.println();

        // 5. Turn LED OFF
        tlc.setPWM(i, 0);
        tlc.write();
        
        // Optional: Small delay to let LED fully discharge if needed
        delay(10);
    }
    Serial.println("--- Sweep Complete ---");
}
