#ifndef LUMOS_SMUX_MAPPING_H
#define LUMOS_SMUX_MAPPING_H

#include <Arduino.h>

/***************************************************************
 * AS7341 SMUX Configuration for Lumos Multi-Wavelength PPG
 * 
 * Reference: AS7341 Application Note AN000666 v1-00
 * 
 * Design principle: For each LED, select the 6 closest spectral
 * photodiodes (always including F1 at 415nm as short-wavelength
 * reference). NO CLEAR channel - only spectral PDs.
 * 
 * AS7341 Photodiode Center Wavelengths:
 *   F1=415nm, F2=445nm, F3=480nm, F4=515nm, F5=555nm,
 *   F6=590nm, F7=630nm, F8=680nm, NIR=910nm
 * 
 * 
 * SMUX Byte Layout (from app note Figure 3):
 * Addr | High Nibble [6:4]     | Low Nibble [2:0]
 * -----|----------------------|------------------
 * 0x00 | Pixel 1  = F3_L      | (unused)
 * 0x01 | (unused)             | Pixel 2  = F1_L
 * 0x02 | (unused)             | (unused)
 * 0x03 | Pixel 7  = F8_L      | (unused)
 * 0x04 | (unused)             | Pixel 8  = F6_L
 * 0x05 | Pixel 11 = F4_L      | Pixel 10 = F2_L
 * 0x06 | Pixel 13 = F5_L      | (unused)
 * 0x07 | (unused)             | Pixel 14 = F7_L
 * 0x08 | Pixel 17 = CLEAR_L   | (unused)
 * 0x09 | Pixel 19 = F5_R      | (unused)
 * 0x0A | (unused)             | Pixel 20 = F7_R
 * 0x0B | (unused)             | (unused)
 * 0x0C | Pixel 25 = F2_R      | (unused)
 * 0x0D | (unused)             | Pixel 26 = F4_R
 * 0x0E | Pixel 29 = F8_R      | Pixel 28 = F6_R
 * 0x0F | Pixel 31 = F3_R      | (unused)
 * 0x10 | Pixel 33 = GPIO_EXT  | Pixel 32 = F1_R
 * 0x11 | Pixel 35 = CLEAR_R   | Pixel 34 = INT_EXT
 * 0x12 | Pixel 37 = DARK      | (unused)
 * 0x13 | Pixel 39 = FLICKER   | Pixel 38 = NIR
 * 
 * ADC Values: 0=disabled, 1=ADC0, 2=ADC1, 3=ADC2, 4=ADC3, 5=ADC4, 6=ADC5
 ***************************************************************/

/***************************************************************
 * Helper Macros
 ***************************************************************/
#define DIS  0x00
#define ADC0 0x01
#define ADC1 0x02
#define ADC2 0x03
#define ADC3 0x04
#define ADC4 0x05
#define ADC5 0x06

#define HI_LO(hi, lo) (((hi) << 4) | (lo))

/***************************************************************
 * LED0 – UV (415nm)
 * LED1 – Royal Blue (450nm)  
 * LED2 – Blue (470nm)
 * LED3 – Green (530nm)
 * 
 * PDs: F1(415), F2(445), F3(480), F4(515), F5(555), F6(590)
 * ADC mapping: ADC0=F1, ADC1=F2, ADC2=F3, ADC3=F4, ADC4=F5, ADC5=F6
 ***************************************************************/
static const uint8_t SMUX_F1_F6[20] = {
    /*0x00*/ HI_LO(ADC2,DIS),   // F3_L -> ADC2
    /*0x01*/ ADC0,              // F1_L -> ADC0
    /*0x02*/ DIS,
    /*0x03*/ HI_LO(DIS,DIS),    // F8_L disabled
    /*0x04*/ HI_LO(DIS,ADC5),   // F6_L -> ADC5
    /*0x05*/ HI_LO(ADC3,ADC1),  // F4_L -> ADC3, F2_L -> ADC1
    /*0x06*/ HI_LO(ADC4,DIS),   // F5_L -> ADC4
    /*0x07*/ HI_LO(DIS,DIS),    // F7_L disabled
    /*0x08*/ DIS,               // CLEAR_L disabled
    /*0x09*/ HI_LO(ADC4,DIS),   // F5_R -> ADC4
    /*0x0A*/ HI_LO(DIS,DIS),    // F7_R disabled
    /*0x0B*/ DIS,
    /*0x0C*/ HI_LO(ADC1,DIS),   // F2_R -> ADC1
    /*0x0D*/ HI_LO(DIS,ADC3),   // F4_R -> ADC3
    /*0x0E*/ HI_LO(DIS,ADC5),   // F8_R disabled, F6_R -> ADC5
    /*0x0F*/ HI_LO(ADC2,DIS),   // F3_R -> ADC2
    /*0x10*/ HI_LO(DIS,ADC0),   // GPIO disabled, F1_R -> ADC0
    /*0x11*/ DIS,               // CLEAR_R disabled
    /*0x12*/ DIS,               // DARK disabled
    /*0x13*/ DIS                // FLICKER disabled, NIR disabled
};

/***************************************************************
 * LED4 – Lime (567nm)
 * 
 * PDs: F1(415), F3(480), F4(515), F5(555), F6(590), F7(630)
 * ADC mapping: ADC0=F1, ADC1=F3, ADC2=F4, ADC3=F5, ADC4=F6, ADC5=F7
 * (Skip F2 - too far from 567nm)
 ***************************************************************/
static const uint8_t SMUX_LED4_LIME[20] = {
    /*0x00*/ HI_LO(ADC1,DIS),   // F3_L -> ADC1
    /*0x01*/ ADC0,              // F1_L -> ADC0
    /*0x02*/ DIS,
    /*0x03*/ HI_LO(DIS,DIS),    // F8_L disabled
    /*0x04*/ HI_LO(DIS,ADC4),   // F6_L -> ADC4
    /*0x05*/ HI_LO(ADC2,DIS),   // F4_L -> ADC2, F2_L disabled
    /*0x06*/ HI_LO(ADC3,DIS),   // F5_L -> ADC3
    /*0x07*/ HI_LO(DIS,ADC5),   // F7_L -> ADC5
    /*0x08*/ DIS,               // CLEAR_L disabled
    /*0x09*/ HI_LO(ADC3,DIS),   // F5_R -> ADC3
    /*0x0A*/ HI_LO(DIS,ADC5),   // F7_R -> ADC5
    /*0x0B*/ DIS,
    /*0x0C*/ HI_LO(DIS,DIS),    // F2_R disabled
    /*0x0D*/ HI_LO(DIS,ADC2),   // F4_R -> ADC2
    /*0x0E*/ HI_LO(DIS,ADC4),   // F8_R disabled, F6_R -> ADC4
    /*0x0F*/ HI_LO(ADC1,DIS),   // F3_R -> ADC1
    /*0x10*/ HI_LO(DIS,ADC0),   // GPIO disabled, F1_R -> ADC0
    /*0x11*/ DIS,               // CLEAR_R disabled
    /*0x12*/ DIS,               // DARK disabled
    /*0x13*/ DIS                // FLICKER disabled, NIR disabled
};

/***************************************************************
 * LED5 – Amber (600nm)
 * 
 * PDs: F1(415), F4(515), F5(555), F6(590), F7(630), F8(680)
 * ADC mapping: ADC0=F1, ADC1=F4, ADC2=F5, ADC3=F6, ADC4=F7, ADC5=F8
 * (Skip F2, F3 - too far from 600nm)
 ***************************************************************/
static const uint8_t SMUX_LED5_AMBER[20] = {
    /*0x00*/ HI_LO(DIS,DIS),    // F3_L disabled
    /*0x01*/ ADC0,              // F1_L -> ADC0
    /*0x02*/ DIS,
    /*0x03*/ HI_LO(ADC5,DIS),   // F8_L -> ADC5
    /*0x04*/ HI_LO(DIS,ADC3),   // F6_L -> ADC3
    /*0x05*/ HI_LO(ADC1,DIS),   // F4_L -> ADC1, F2_L disabled
    /*0x06*/ HI_LO(ADC2,DIS),   // F5_L -> ADC2
    /*0x07*/ HI_LO(DIS,ADC4),   // F7_L -> ADC4
    /*0x08*/ DIS,               // CLEAR_L disabled
    /*0x09*/ HI_LO(ADC2,DIS),   // F5_R -> ADC2
    /*0x0A*/ HI_LO(DIS,ADC4),   // F7_R -> ADC4
    /*0x0B*/ DIS,
    /*0x0C*/ HI_LO(DIS,DIS),    // F2_R disabled
    /*0x0D*/ HI_LO(DIS,ADC1),   // F4_R -> ADC1
    /*0x0E*/ HI_LO(ADC5,ADC3),  // F8_R -> ADC5, F6_R -> ADC3
    /*0x0F*/ HI_LO(DIS,DIS),    // F3_R disabled
    /*0x10*/ HI_LO(DIS,ADC0),   // GPIO disabled, F1_R -> ADC0
    /*0x11*/ DIS,               // CLEAR_R disabled
    /*0x12*/ DIS,               // DARK disabled
    /*0x13*/ DIS                // FLICKER disabled, NIR disabled
};

/***************************************************************
 * LED6 – Red (630nm)
 * LED7 – Deep Red (660nm)
 * 
 * PDs: F1(415), F5(555), F6(590), F7(630), F8(680), NIR(910)
 * ADC mapping: ADC0=F1, ADC1=F5, ADC2=F6, ADC3=F7, ADC4=F8, ADC5=NIR
 * (Skip F2, F3, F4 - too far from 630-660nm)
 ***************************************************************/
static const uint8_t SMUX_LED6_LED7_RED[20] = {
    /*0x00*/ HI_LO(DIS,DIS),    // F3_L disabled
    /*0x01*/ ADC0,              // F1_L -> ADC0
    /*0x02*/ DIS,
    /*0x03*/ HI_LO(ADC4,DIS),   // F8_L -> ADC4
    /*0x04*/ HI_LO(DIS,ADC2),   // F6_L -> ADC2
    /*0x05*/ HI_LO(DIS,DIS),    // F4_L disabled, F2_L disabled
    /*0x06*/ HI_LO(ADC1,DIS),   // F5_L -> ADC1
    /*0x07*/ HI_LO(DIS,ADC3),   // F7_L -> ADC3
    /*0x08*/ DIS,               // CLEAR_L disabled
    /*0x09*/ HI_LO(ADC1,DIS),   // F5_R -> ADC1
    /*0x0A*/ HI_LO(DIS,ADC3),   // F7_R -> ADC3
    /*0x0B*/ DIS,
    /*0x0C*/ HI_LO(DIS,DIS),    // F2_R disabled
    /*0x0D*/ HI_LO(DIS,DIS),    // F4_R disabled
    /*0x0E*/ HI_LO(ADC4,ADC2),  // F8_R -> ADC4, F6_R -> ADC2
    /*0x0F*/ HI_LO(DIS,DIS),    // F3_R disabled
    /*0x10*/ HI_LO(DIS,ADC0),   // GPIO disabled, F1_R -> ADC0
    /*0x11*/ DIS,               // CLEAR_R disabled
    /*0x12*/ DIS,               // DARK disabled
    /*0x13*/ HI_LO(DIS,ADC5)    // FLICKER disabled, NIR -> ADC5
};

/***************************************************************
 * LED8 – IR1 (850nm)
 * LED9 – IR2 (940nm)
 * 
 * PDs: F1(415), F5(555), F6(590), F7(630), F8(680), NIR(910)
 * ADC mapping: ADC0=F1, ADC1=F5, ADC2=F6, ADC3=F7, ADC4=F8, ADC5=NIR
 * (Same as RED - best coverage for IR wavelengths)
 ***************************************************************/
static const uint8_t SMUX_LED8_LED9_IR[20] = {
    /*0x00*/ HI_LO(DIS,DIS),    // F3_L disabled
    /*0x01*/ ADC0,              // F1_L -> ADC0
    /*0x02*/ DIS,
    /*0x03*/ HI_LO(ADC4,DIS),   // F8_L -> ADC4
    /*0x04*/ HI_LO(DIS,ADC2),   // F6_L -> ADC2
    /*0x05*/ HI_LO(DIS,DIS),    // F4_L disabled, F2_L disabled
    /*0x06*/ HI_LO(ADC1,DIS),   // F5_L -> ADC1
    /*0x07*/ HI_LO(DIS,ADC3),   // F7_L -> ADC3
    /*0x08*/ DIS,               // CLEAR_L disabled
    /*0x09*/ HI_LO(ADC1,DIS),   // F5_R -> ADC1
    /*0x0A*/ HI_LO(DIS,ADC3),   // F7_R -> ADC3
    /*0x0B*/ DIS,
    /*0x0C*/ HI_LO(DIS,DIS),    // F2_R disabled
    /*0x0D*/ HI_LO(DIS,DIS),    // F4_R disabled
    /*0x0E*/ HI_LO(ADC4,ADC2),  // F8_R -> ADC4, F6_R -> ADC2
    /*0x0F*/ HI_LO(DIS,DIS),    // F3_R disabled
    /*0x10*/ HI_LO(DIS,ADC0),   // GPIO disabled, F1_R -> ADC0
    /*0x11*/ DIS,               // CLEAR_R disabled
    /*0x12*/ DIS,               // DARK disabled
    /*0x13*/ HI_LO(DIS,ADC5)    // FLICKER disabled, NIR -> ADC5
};

static const uint8_t* SMUX_CONFIGS[11] = {
    SMUX_F1_F6,           // LED0  – UV (415nm)
    SMUX_F1_F6,           // LED1  – Royal Blue (450nm)
    SMUX_F1_F6,           // LED2  – Blue (470nm)
    SMUX_F1_F6,           // LED3  – Cyan (500nm)
    SMUX_F1_F6,           // LED4  – Green (530nm)
    SMUX_LED4_LIME,       // LED5  – Lime (567nm)
    SMUX_LED5_AMBER,      // LED6  – Amber (599nm)
    SMUX_LED6_LED7_RED,   // LED7  – Red (633nm)
    SMUX_LED6_LED7_RED,   // LED8  – Deep Red (660nm)
    SMUX_LED8_LED9_IR,    // LED9  – IR1 (850nm)
    SMUX_LED8_LED9_IR     // LED10 – IR2 (940nm)
};

#endif