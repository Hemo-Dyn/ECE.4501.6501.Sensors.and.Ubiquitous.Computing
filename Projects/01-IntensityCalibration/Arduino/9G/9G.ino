/* ###################     0. PD, LED Driver library     ################### */

#include <Wire.h>
#include <Adafruit_AS7341.h>
#include "Adafruit_TLC59711.h"

/* ###################     1. LED DRIVER SETUP INITIALIZATION     ################### */

// TLC5947 is a 24-channel 12-bit PWM LED driver.
// NUM_TLC5947 = number of chained boards (each adds 24 channels).
#define NUM_TLC59711 1

// TLC5947 wiring pins (board-specific naming; these must match your MCU pin map).
#define DATA_PIN  10
#define CLOCK_PIN 8
bool serialDebug = false;  // device will serial output all the data

// ---- Serial debug helpers ----
#define DBG_BEGIN(baud) do { if (serialDebug) Serial.begin(baud); } while (0)
#define DBG_PRINT(x)    do { if (serialDebug) Serial.print(x); } while (0)
#define DBG_PRINTLN(x)  do { if (serialDebug) Serial.println(x); } while (0)
#define DBG_WRITE(x)    do { if (serialDebug) Serial.write(x); } while (0)
// Create TLC5947 instance. Constructor order is (numBoards, clk, data, latch).
Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, CLOCK_PIN, DATA_PIN);

/* ###################     2. LUMOS DEVICE INITIALIZATION         ################### */

// Photodiode channels (AS7341) used:
const int totalPDNum = 10;

// Number of LEDs actively used (<= 24 for a single TLC5947 board).
const int ledNum = 9;

// AS7341 integration settings.
// ATIME/ASTEP determine integration time; see AS7341 datasheet / Adafruit docs.
const int atime = 50;
const int astep = 499;

/* ###################     2.1 DELAYS                             ################### */

// Delay after turning an LED on before reading sensors.
// Should be long enough for LED + sensor integration to be valid.
const int ledOnDelay = 20;

// Delay at end of setup to give hardware time to stabilize.
const int startDelay = 3000;

/* ###################     2.2 LIGHTS UP SEQUENCE                 ################### */

// Grouping / ordering of LED activation.
// Here, groups correspond 1:1 with LEDs (no multi-LED groups).
const int numGroups = 9;
int ledLoopCounter = 0;

// How often to refresh ambient readings (measured with all LEDs off).
// Every ambientAdjustFreq LEDs, the code measures ambient and subtracts it from LED-on readings.
const int ambientAdjustFreq = 10;

// When subtracting ambient, clamp negative values up to this.
// If you want to allow negatives, you’d need signed storage; this code keeps uint16_t data.
const int negativeAmbient = 0;

// Activation order for LEDs. If you want a custom sweep pattern, reorder this list.
const int ledOrder[numGroups] = {
    8, 0, 1, 2, 3, 4, 5, 6, 7
};

// Index and LED wavelength map:

// "LEDIndexLightOrder":       [    0,    1,    2,    3,    4,    5,    6,    7,    8],
// "LEDIndexToWavelength":     [  660,  633,  599,  567,  530,  470,  450,  415,  940],

/* ###################     3. VISIBLE PD INITIALIZATION & READINGS PLACEHOLDER      ################### */

// AS7341 spectral sensor object and settings.
Adafruit_AS7341 as7341;
as7341_gain_t gain = AS7341_GAIN_256X;

// Delay used when AS7341 is missing at boot (tight loop).
const int findAS7341Delay = 1000;

// Storage for last LED-on readings.
uint16_t readings[totalPDNum] = {0};

/* ###################     FORWARD DECLS (optional but clean)     ################### */

// LED helper
void allLedsOff();

// Data formatting
String combineData(uint16_t readings[], int led, int pwm);

/*
  allLedsOff():
  - Ensures the TLC5947 is initialized and drives all used LED channels to 0 PWM.
  - This is called during setup and whenever USB is detected.
*/
void allLedsOff() {
    tlc.begin();
    for (int i = 0; i < ledNum; i++) {
        tlc.setPWM(i, 0);
    }
    tlc.write();
}

void setup() {
    allLedsOff();
    if (!as7341.begin()) {
        DBG_PRINTLN("Could not find AS7341");
        while (1) { delay(findAS7341Delay); }
    }
    // Configure AS7341 integration and gain.
    as7341.setATIME(atime);
    as7341.setASTEP(astep);
    as7341.setGain(gain);    
    tlc.begin();
    delay(startDelay);
}

void loop() {
    for (int g = 0; g < numGroups; g++) {
        // Select the LED index for this iteration based on ledOrder.
        int led = ledOrder[g];
        /*
            Measurement pass for this LED:
            - Turn LED on at final PWM
            - Read sensors, subtract ambient
            - Log record
            - Turn LED off
        */
        tlc.setPWM(led, ledBrightness[led]);
        tlc.write();
        delay(ledOnDelay);

        if (!as7341.readAllChannels()) {
        DBG_PRINTLN("Error reading all channels!");
        return;
        }

        readings[0]  = (int)as7341.getChannel(AS7341_CHANNEL_415nm_F1);
        readings[1]  = (int)as7341.getChannel(AS7341_CHANNEL_445nm_F2);
        readings[2]  = (int)as7341.getChannel(AS7341_CHANNEL_480nm_F3);
        readings[3]  = (int)as7341.getChannel(AS7341_CHANNEL_515nm_F4);
        readings[4]  = (int)as7341.getChannel(AS7341_CHANNEL_555nm_F5);
        readings[5]  = (int)as7341.getChannel(AS7341_CHANNEL_590nm_F6);
        readings[6]  = (int)as7341.getChannel(AS7341_CHANNEL_630nm_F7);
        readings[7]  = (int)as7341.getChannel(AS7341_CHANNEL_680nm_F8);
        readings[8]  = (int)as7341.getChannel(AS7341_CHANNEL_NIR);
        readings[9]  = (int)as7341.getChannel(AS7341_CHANNEL_CLEAR);

        String pdData = combineData(readings, led, ledBrightness[led]);

        // Turn LED off after logging.
        tlc.setPWM(led, 0);
        tlc.write();
        delay(ledOnDelay);

        // Count how many LEDs we’ve processed since the last ambient update.
        ledLoopCounter++;

    }
}

String combineData(uint16_t readings[], int led, int pwm) {
    String pdData = "";

    pdData += String(timeBuf) + ",";

    // Append all PD channels
    for (int i = 0; i < totalPDNum; i++) {
        pdData += String(readings[i]) + ",";
    }

    // Append LED index in brackets (easy to parse)
    pdData += "[";
    pdData += String(led);
    pdData += "],";

    // Append PWM
    pdData += String(pwm);
    pdData += ";";

    return pdData;
}