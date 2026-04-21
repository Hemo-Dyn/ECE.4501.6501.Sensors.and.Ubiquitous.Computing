#include "FastPD.hpp"

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
FastPD::FastPD() : Adafruit_AS7341()
{
    diodeConfig = DEFAULT_DIODES;
    setATIME(A_TIME);
    setASTEP(A_STEP);
    setGain((as7341_gain_t)AS7341_GAIN_256X);
}

bool FastPD::loadSMUXCustomTable(const uint8_t table[20]) {
    enableSpectralMeasurement(false);
    for (uint8_t i = 0; i < 20; i++) {
        writeRegister(0x00 + i, table[i]);   
    }
    writeRegister(AS7341_CFG6, 0x18);
    return enableSMUX();
    
}

void FastPD::update_readings()
{
    int idx = 0;
    for (bool diode : diodeConfig) {
        if (diode) {
            diodeReading[idx] = (int) getChannel((as7341_color_channel_t)idx);
        }
        idx++;
    }
}

void FastPD::writeRegister(byte addr, byte val) {
  Adafruit_BusIO_Register reg = Adafruit_BusIO_Register(i2c_dev, addr);
  reg.write(val);
}

bool FastPD::enableSMUX(void) {

    Adafruit_BusIO_Register enable_reg =
        Adafruit_BusIO_Register(i2c_dev, AS7341_ENABLE);
    Adafruit_BusIO_RegisterBits smux_enable_bit =
        Adafruit_BusIO_RegisterBits(&enable_reg, 1, 4);
    bool success = smux_enable_bit.write(true);

    int timeOut = 1000; // Arbitrary value, but if it takes 1000 milliseconds then
                        // something is wrong
    int count = 0;
    while (smux_enable_bit.read() && count < timeOut) {
        delay(1);
        count++;
    }
    if (count >= timeOut)
        return false;
    else
        return success;
}