#pragma once

// ---- Serial debug helpers ----
#define DBG_BEGIN(baud)         \
    do                          \
    {                           \
        if (serialDebug)        \
            Serial.begin(baud); \
    } while (0)
#define DBG_PRINT(x)         \
    do                       \
    {                        \
        if (serialDebug)     \
            Serial.print(x); \
    } while (0)
#define DBG_PRINTLN(x)         \
    do                         \
    {                          \
        if (serialDebug)       \
            Serial.println(x); \
    } while (0)
#define DBG_WRITE(x)         \
    do                       \
    {                        \
        if (serialDebug)     \
            Serial.write(x); \
    } while (0)
