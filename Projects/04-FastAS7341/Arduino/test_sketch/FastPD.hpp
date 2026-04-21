#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_AS7341.h>
#include "Adafruit_TinyUSB.h"
#include <algorithm>
#include <string.h>
#include <array>

#define LED_NUM 9
#define TOTAL_PD 10

#define A_TIME      50
#define A_STEP      499
#define LED_DELAY   20
#define START_DELAY 3000

/*
 * Color channel enum:
 *AS7341_CHANNEL_415nm_F1,
  AS7341_CHANNEL_445nm_F2,
  AS7341_CHANNEL_480nm_F3,
  AS7341_CHANNEL_515nm_F4,
  AS7341_CHANNEL_CLEAR_0,
  AS7341_CHANNEL_NIR_0,
  AS7341_CHANNEL_555nm_F5,
  AS7341_CHANNEL_590nm_F6,
  AS7341_CHANNEL_630nm_F7,
  AS7341_CHANNEL_680nm_F8,
  AS7341_CHANNEL_CLEAR,
  AS7341_CHANNEL_NIR,
 */

const std::array<bool, 12> DEFAULT_DIODES = {1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0};

class FastPD: public Adafruit_AS7341 {
public:
    FastPD();

    bool loadSMUXCustomTable(const u_int8_t table[20]);
    void update_readings();
    int readings[12];
    void writeRegister(byte addr, byte val); 
    bool enableSMUX();

private: 
    std::array<bool, 12> diodeConfig;
    std::array<int, 12 > diodeReading;
};