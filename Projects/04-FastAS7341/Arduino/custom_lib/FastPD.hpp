#pragma once

#include "Arduino.h"
#include "inc/FastPD.hpp"
#include "Adafruit_AS7341.h"
#include <string.h>

#define LED_NUM 9
#define TOTAL_PD 10
#define DEFAULT_ORDER_LED {0,1,2,3,4,5,6,7,8}
#define DEFAULT_ORDER_BRIGHT {3000, 5000, 8000, 8000, 8000, 8000, 8000, 8000, 1200}

#define A_TIME      50
#define A_STEP      499
#define LED_DELAY   20
#define START_DELAY 3000



class FastPD: public Adafruit_AS7341() {
public:
    FastPD();

    bool loadSMUXCustomTable(const u_int8_t table[20]);
    void FastPD::update_readings();
    int readings[LED_NUM+1];

private:
    uint8_t ledOrder[LED_NUM + 1];
    uint8_t ledBrightness[LED_NUM + 1];

    constexpr uint8_t DEFAULT_ledOrder[LED_NUM + 1] = DEFAULT_ORDER_LED;
    constexpr uint8_t DEFAULT_ledBrightness[LED_NUM + 1] = DEFAULT_ORDER_BRIGHT;
}