#pragma once
#include "Arduino.h"
#include <stdint.h>
#include <Adafruit_AS7341.h>

#define TOTAL_PD_CHANNELS 10 // Photodiode channels (AS7341) used
#define LED_NUM 9            // number of LEDs actively used

#define A_TIME 50 // ATIME/ASTEP determine integration time; see AS7341 datasheet / Adafruit docs.
#define A_STEP 499

#define FIND_PD_DELAY 1000   // delay when PD can't be found for whatever reason
#define LED_ON_DELAY 20      // delay after turning on to start reading (ms)
#define LED_START_DELAY 3000 // delay upon power up when pd just sends garbage on i2c buss

#define GROUPS 9         // groups mean simultaneous LED : LTF scans, but here we keep 1:1 so only one pd is measuring at 1 LED source
#define ADJUST_FREQ 10   // how often to refresh ambient readings
#define AMBIENT_OFFSET 0 // offset of ambient light to subtract by

static constexpr uint8_t DEFAULT_ledOrder[] = {8, 0, 1, 2, 3, 4, 5, 6, 7}; // Activation order for LEDs. If you want a custom sweep pattern, reorder this list.
static constexpr int DEFAULT_ledBrightness[] = {3000, 5000, 8000, 8000, 8000, 8000, 8000, 8000, 1200};
static constexpr as7341_color_channel_t pdColorChannels[] = {
    AS7341_CHANNEL_415nm_F1,
    AS7341_CHANNEL_445nm_F2,
    AS7341_CHANNEL_480nm_F3,
    AS7341_CHANNEL_515nm_F4,
    AS7341_CHANNEL_555nm_F5,
    AS7341_CHANNEL_590nm_F6,
    AS7341_CHANNEL_630nm_F7,
    AS7341_CHANNEL_680nm_F8,
    AS7341_CHANNEL_NIR,
    AS7341_CHANNEL_CLEAR,
};

class FastPD : public Adafruit_AS7341
{
public:
    FastPD();
    uint16_t readings[LED_NUM + 1];
    uint8_t ledOrder[GROUPS];
    int ledBrightness[LED_NUM];

    void set_order(const uint8_t *order);
    void update_readings();
};