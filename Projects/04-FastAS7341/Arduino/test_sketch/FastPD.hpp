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
  AS7341_CHANNEL_555nm_F5,
  AS7341_CHANNEL_590nm_F6,
  AS7341_CHANNEL_630nm_F7,
  AS7341_CHANNEL_680nm_F8,
  AS7341_CHANNEL_CLEAR,
  AS7341_CHANNEL_NIR,
 */

const std::array<bool, 10> DEFAULT_DIODES = {1, 1, 1, 1, 0, 0, 0, 0, 0, 1};
struct Wiring {
    uint8_t regIndex;
    bool isHighNibble;
};

static const Wiring hardwareMap[10] = {
    {0x01, false}, // F1_L    (Pixel ID 2)  -> Reg 0x01, Bits [2:0]
    {0x05, false}, // F2_L    (Pixel ID 10) -> Reg 0x05, Bits [2:0]
    {0x00, true},  // F3_L    (Pixel ID 1)  -> Reg 0x00, Bits [6:4]
    {0x05, true},  // F4_L    (Pixel ID 11) -> Reg 0x05, Bits [6:4]
    {0x06, true},  // F5_L    (Pixel ID 13) -> Reg 0x06, Bits [6:4]
    {0x04, false}, // F6_L    (Pixel ID 8)  -> Reg 0x04, Bits [2:0]
    {0x07, false}, // F7_L    (Pixel ID 14) -> Reg 0x07, Bits [2:0]
    {0x03, true},  // F8_L    (Pixel ID 7)  -> Reg 0x03, Bits [6:4]
    {0x08, true},  // Clear_L (Pixel ID 17) -> Reg 0x08, Bits [6:4]
    {0x13, false}  // NIR_L   (Pixel ID 38) -> Reg 0x13, Bits [2:0]
};

class FastPD: public Adafruit_AS7341 {
public:
    FastPD();
    void update_readings();
    void init();
    uint16_t readings[10];
    
    void writeRegister(byte addr, byte val); 
    bool loadSMUXCustomTable(const u_int8_t table[20]);
    bool loadDynamicSMUX();
    bool enableSMUX();

private: 
    std::array<bool, 10> diodeConfig;
};