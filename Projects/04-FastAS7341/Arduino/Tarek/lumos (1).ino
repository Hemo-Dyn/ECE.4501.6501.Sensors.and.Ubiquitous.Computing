#include <Adafruit_TinyUSB.h>
#include <Adafruit_TLC59711.h>
#include <Adafruit_AS7341.h>
#include <Wire.h>

#include "Lumos_SMUX_Mapping.h" 

#define NUM_TLC59711 1
#define PIN_DATA  9
#define PIN_CLK   8

Adafruit_TLC59711 tlc(NUM_TLC59711, PIN_CLK, PIN_DATA);
Adafruit_AS7341   as7341;

const uint8_t NUM_LEDS = 11;
const uint8_t SMUX_MAP[NUM_LEDS] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};


// Lighter skin tones
uint16_t ledIntensities[NUM_LEDS] = {
  200,  // LED0  - IR2 (940nm)
  1000,  // LED1  - IR1 (850nm)
  4000,   // LED2  - Deep Red (660nm)
  5000,  // LED3  - Red (633nm)
  40000,  // LED4  - Amber (599nm)
  15000,  // LED5  - Lime (567nm)
  20000,  // LED6  - Green (530nm)
  15000,  // LED7  - Cyan (500nm)
  10000,  // LED8  - Blue (470nm)
  20000,  // LED9  - Royal Blue (450nm)
  20000   // LED10 - UV (415nm)
};


/*
// Darker skin tones
uint16_t ledIntensities[NUM_LEDS] = {
  500,  // LED0  - IR2 (940nm)
  1000,  // LED1  - IR1 (850nm)
  5000,  // LED2  - Deep Red (660nm)
  10000,  // LED3  - Red (633nm)
  40000,  // LED4  - Amber (599nm)
  50000,  // LED5  - Lime (567nm)
  50000,  // LED6  - Green (530nm)
  20000,  // LED7  - Cyan (500nm)
  20000,  // LED8  - Blue (470nm)
  20000,  // LED9  - Royal Blue (450nm)
  20000   // LED10 - UV (415nm)
};
*/

uint8_t  ATIME = 20;
uint16_t ASTEP = 569;
as7341_gain_t GAIN = AS7341_GAIN_512X;

enum State { IDLE, RUNNING };
State currentState = IDLE;

uint8_t ledIndex = 0;
unsigned long lastSwitchTime = 0;
const unsigned long LED_DURATION_MS = 12000;

const uint8_t CMD_START = 0xAA;
const uint8_t CMD_STOP = 0xCC;
const uint8_t CMD_CYCLE_COMPLETE = 0xBB;
const uint8_t CMD_READY = 0xA5;

void allLedsOff() {
  for (int i = 0; i < NUM_TLC59711 * 12; i++) {
    tlc.setPWM(i, 0);
  }
  tlc.write();
}

void turnOnLED(uint8_t idx) {
  allLedsOff();
  tlc.setPWM(idx, ledIntensities[idx]);
  tlc.write();
}

void setup() {
  Serial.begin(2000000);
  delay(100);

  tlc.begin();
  allLedsOff();

  if (!as7341.begin()) {
    Serial.println("AS7341 not found!");
    while (1) delay(500);
  }

  Wire.setClock(1000000);

  as7341.powerEnable(true);
  as7341.enableLED(false);

  as7341.setATIME(ATIME);
  as7341.setASTEP(ASTEP);
  as7341.setGain(GAIN);

  if (!as7341.loadSMUXCustomTable(SMUX_CONFIGS[SMUX_MAP[0]])) {
    Serial.println("Initial SMUX load failed!");
    while (1) delay(1000);
  }

  Serial.write(CMD_READY);
  Serial.flush();

  delay(50);
}

void loop() {
  uint16_t readings[6];
  unsigned long now = millis();

  if (Serial.available()) {
    uint8_t cmd = Serial.read();
    if (cmd == CMD_START && currentState == IDLE) {
      ledIndex = 0;
      lastSwitchTime = now;
      turnOnLED(ledIndex);
      as7341.loadSMUXCustomTable(SMUX_CONFIGS[SMUX_MAP[ledIndex]]);
      currentState = RUNNING;
    }
    else if (cmd == CMD_STOP && currentState == RUNNING) {
      allLedsOff();
      currentState = IDLE;
    }
  }

  if (currentState == IDLE) {
    return;
  }

  if (now - lastSwitchTime >= LED_DURATION_MS) {
    lastSwitchTime = now;
    ledIndex++;

    if (ledIndex >= NUM_LEDS) {
      allLedsOff();
      Serial.write(CMD_CYCLE_COMPLETE);
      currentState = IDLE;
      return;
    }

    turnOnLED(ledIndex);
    as7341.loadSMUXCustomTable(SMUX_CONFIGS[SMUX_MAP[ledIndex]]);
  }

  as7341.enableSpectralMeasurement(true);
  as7341.delayForData(0);

  readings[0] = as7341.readChannel(AS7341_ADC_CHANNEL_0);
  readings[1] = as7341.readChannel(AS7341_ADC_CHANNEL_1);
  readings[2] = as7341.readChannel(AS7341_ADC_CHANNEL_2);
  readings[3] = as7341.readChannel(AS7341_ADC_CHANNEL_3);
  readings[4] = as7341.readChannel(AS7341_ADC_CHANNEL_4);
  readings[5] = as7341.readChannel(AS7341_ADC_CHANNEL_5);

  Serial.write(&ledIndex, 1);
  Serial.write((uint8_t*)readings, sizeof(readings));
}