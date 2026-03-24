#include "Arduino.h"
#include "inc/FastPD.hpp"
#include "Adafruit_AS7341.h"
#include <string.h>

// Index and LED wavelength map:
// "LEDIndexLightOrder":       [    0,    1,    2,    3,    4,    5,    6,    7,    8],
// "LEDIndexToWavelength":     [  660,  633,  599,  567,  530,  470,  450,  415,  940],

FastPD::FastPD() : Adafruit_AS7341()
{

    memcpy(ledOrder, DEFAULT_ledOrder, sizeof(ledOrder));
    memcpy(ledBrightness, DEFAULT_ledBrightness, sizeof(ledOrder));

    setATIME(A_TIME);
    setASTEP(A_STEP);
    setGain((as7341_gain_t)AS7341_GAIN_256X);
}

bool FastPD::loadSMUXCustomTable(const uint8_t table[20]) {

    // 1. Disable spectral measurement
    enableSpectralMeasurement(false);

    // 2. Write all 20 SMUX bytes to RAM FIRST
    for (uint8_t i = 0; i < 20; i++) {
        writeRegister(0x00 + i, table[i]);   // SMUX RAM starts at 0x00
    }

    // 3. NOW set SMUX command to WRITE (copies RAM -> SMUX chain)
    setSMUXCommand(AS7341_SMUX_CMD_WRITE);

    // 4. Commit SMUX config (sets SMUXEN bit to execute the command)
    return enableSMUX();
}

void FastPD::update_readings()
{

    if (!readAllChannels())
    {
        Serial.println("Error: AS7341 failed to read channels");
        return;
    }

    for (int i = 0; i < LED_NUM + 1; i++)
    {
        readings[i] = (int)getChannel(pdColorChannels[i]);
    }
}