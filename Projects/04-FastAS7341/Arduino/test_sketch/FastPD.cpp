#include "FastPD.hpp"

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
FastPD::FastPD() : Adafruit_AS7341()
{
    diodeConfig = DEFAULT_DIODES;
}

void FastPD::init() {
    setATIME(A_TIME);
    setASTEP(A_STEP);
    setGain((as7341_gain_t)AS7341_GAIN_256X);
}

bool FastPD::loadDynamicSMUX() {
    uint8_t table[20];
    memset(table, 0, sizeof(table));
    int assignedADC = 0;

    for (int i=0; i<10; i++) {
        if (diodeConfig[i]) {
            if (assignedADC >= 6) return false;
            uint8_t adcValue = assignedADC + 1;
            Wiring w = hardwareMap[i];
            if (w.isHighNibble) {
                table[w.regIndex] |= (adcValue << 4);
            } else {
                table[w.regIndex] |= adcValue;
            }
            assignedADC++;
        }
    }
    return loadSMUXCustomTable(table);
}

bool FastPD::loadSMUXCustomTable(const uint8_t table[20]) {
    enableSpectralMeasurement(false);
    for (uint8_t i = 0; i < 20; i++) {
        writeRegister(0x00 + i, table[i]);   
    }
    writeRegister(AS7341_CFG6, 0x10); // enables write mode for smux
    return enableSMUX();   
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
        enableSpectralMeasurement(true);
        return success;
}

void FastPD::update_readings() {
    // Target the first data register (CH0_DATA_L at 0x95)
    Adafruit_BusIO_Register data_reg = Adafruit_BusIO_Register(i2c_dev, AS7341_CH0_DATA_L, 2);
    
    uint16_t raw_buffer[6]; // Temporary storage for the 6 active ADCs
    data_reg.read((uint8_t *)raw_buffer, 12); // Read all 6 channels in one go

    int adc_ptr = 0;
    memset(readings, 0, sizeof(readings)); // Clear old data

    for (int i = 0; i < 10; i++) {
        if (diodeConfig[i] && adc_ptr < 6) {
            // Map the ADC result to the correct photodiode index
            readings[i] = raw_buffer[adc_ptr];
            adc_ptr++;
        }
    }
}

void FastPD::writeRegister(byte addr, byte val) {
  Adafruit_BusIO_Register reg = Adafruit_BusIO_Register(i2c_dev, addr);
  reg.write(val);
}