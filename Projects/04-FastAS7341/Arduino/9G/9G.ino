/* ###################     0. PD, LED Driver library     ################### */

#include <Wire.h>
#include <Adafruit_TinyUSB.h>
#include "Adafruit_TLC59711.h"

#include "inc/FastPD.hpp"
#include "inc/Debug.hpp"

/* ###################     1. LED DRIVER SETUP INITIALIZATION     ################### */

// TLC5947 is a 24-channel 12-bit PWM LED driver.
#define NUM_TLC59711 1

// TLC5947 wiring pins (board-specific naming; these must match your MCU pin map).
#define DATA_PIN 10
#define CLOCK_PIN 8
bool serialDebug = true; // device will serial output all the data

Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, CLOCK_PIN, DATA_PIN); // Create TLC5947 instance. Constructor order is (numBoards, clk, data, latch)

/* ###################     3. VISIBLE PD INITIALIZATION & READINGS PLACEHOLDER      ################### */
// Adafruit_AS7341 as7341;
FastPD phastPhoto;

void allLedsOff();

String combineData(uint16_t readings[], int led, int pwm);

/*
  allLedsOff():
  - Ensures the TLC5947 is initialized and drives all used LED channels to 0 PWM.
  - This is called during setup and whenever USB is detected.
*/
void allLedsOff()
{
    tlc.begin();
    for (int i = 0; i < LED_NUM; i++)
    {
        tlc.setPWM(i, 0);
    }
    tlc.write();
}

void setup()
{
    allLedsOff();
    Wire.begin();
    DBG_BEGIN(9600);

    while (!Serial)
        ;

    if (!phastPhoto.begin())
    {
        DBG_PRINTLN("Could not find AS7341");
        while (1)
        {
            delay(FIND_PD_DELAY);
        }
    }

    delay(LED_START_DELAY);
}


int ledLoopCounter;
void loop()
{
    for (int g = 0; g < GROUPS; g++)
    {
        // Select the LED index for this iteration based on ledOrder.
        int led = phastPhoto.ledOrder[g];
        /*
            Measurement pass for this LED:
            - Turn LED on at final PWM
            - Read sensors, subtract ambient
            - Log record
            - Turn LED off
        */
        tlc.setPWM(led, phastPhoto.ledBrightness[led]);
        tlc.write();
        delay(LED_ON_DELAY);

        phastPhoto.update_readings();

        String pdData = combineData(phastPhoto.readings, led, phastPhoto.ledBrightness[led]);
        DBG_PRINTLN(pdData);

        // Turn LED off after logging.
        tlc.setPWM(led, 0);
        tlc.write();
        delay(LED_ON_DELAY);

        ledLoopCounter++;       // Count how many LEDs we’ve processed since the last ambient update.
    }
}

String combineData(uint16_t readings[], int led, int pwm)
{
    String pdData = "";

    // Time since boot (HH-MM-SS.mmm)
    unsigned long ms = millis();
    unsigned long totalSeconds = ms / 1000UL;

    unsigned int hours = totalSeconds / 3600UL;
    unsigned int minutes = (totalSeconds % 3600UL) / 60UL;
    unsigned int seconds = totalSeconds % 60UL;
    unsigned int msec = ms % 1000UL;

    char timeBuf[16];
    snprintf(timeBuf, sizeof(timeBuf), "%02u-%02u-%02u.%03u",
             hours, minutes, seconds, msec);

    // Prefix timestamp
    pdData += String(timeBuf) + ",";

    // Append all PD channels
    for (int i = 0; i < TOTAL_PD_CHANNELS; i++)
    {
        pdData += String(readings[i]) + ",";
    }

    // Append LED index in brackets (easy to parse)
    pdData += "[";
    pdData += String(led);
    pdData += "],";

    // Append PWM
    pdData += String(pwm);

    // Record terminator (used by readback() to print one record per line)
    pdData += ";";

    return pdData;
}