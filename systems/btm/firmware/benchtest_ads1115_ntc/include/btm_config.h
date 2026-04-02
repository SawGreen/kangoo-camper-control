#pragma once
#include <stdint.h>

// Bench configuration for ESP32 Dev Module + 4x ADS1115 + 16x NTC.
// Keep this intentionally simple for tomorrow's hardware session.

#ifndef BTM_SERIAL_BAUD
#define BTM_SERIAL_BAUD 115200
#endif

#ifndef BTM_PRINT_PERIOD_MS
#define BTM_PRINT_PERIOD_MS 1000
#endif

// ESP32 Dev Module practical defaults
#ifndef BTM_I2C_SDA_PIN
#define BTM_I2C_SDA_PIN 21
#endif

#ifndef BTM_I2C_SCL_PIN
#define BTM_I2C_SCL_PIN 22
#endif

// Conservative default for first bring-up. Increase later if needed.
#ifndef BTM_I2C_FREQ_HZ
#define BTM_I2C_FREQ_HZ 100000
#endif

static constexpr uint8_t BTM_ADS_COUNT = 4;
static constexpr uint8_t BTM_CHANNELS_PER_ADS = 4;
static constexpr uint8_t BTM_CHANNEL_COUNT = BTM_ADS_COUNT * BTM_CHANNELS_PER_ADS;

// ADDR strap plan for 4 devices
static constexpr uint8_t BTM_ADS_ADDRS[BTM_ADS_COUNT] = {0x48, 0x49, 0x4A, 0x4B};

// ADS1115 gain is set to GAIN_ONE in main.cpp (FS = +/-4.096V)
#ifndef BTM_ADS_FS_VOLTS
#define BTM_ADS_FS_VOLTS 4.096f
#endif

#ifndef BTM_DIVIDER_VCC_VOLTS
#define BTM_DIVIDER_VCC_VOLTS 3.300f
#endif

#ifndef BTM_DIVIDER_R_FIXED_OHM
#define BTM_DIVIDER_R_FIXED_OHM 10000.0f
#endif

enum BtmDividerOrientation : uint8_t {
  BTM_R_FIXED_TOP__NTC_BOTTOM = 0,
  BTM_NTC_TOP__R_FIXED_BOTTOM = 1
};

#ifndef BTM_DIVIDER_ORIENTATION
#define BTM_DIVIDER_ORIENTATION BTM_R_FIXED_TOP__NTC_BOTTOM
#endif

// NTC Beta model defaults for 10k B3950
#ifndef BTM_NTC_BETA_K
#define BTM_NTC_BETA_K 3950.0f
#endif

#ifndef BTM_NTC_R25_OHM
#define BTM_NTC_R25_OHM 10000.0f
#endif

#ifndef BTM_NTC_T0_C
#define BTM_NTC_T0_C 25.0f
#endif

// Simple bench flags
#ifndef BTM_SENSOR_SHORT_FRAC
#define BTM_SENSOR_SHORT_FRAC 0.05f
#endif

#ifndef BTM_SENSOR_OPEN_FRAC
#define BTM_SENSOR_OPEN_FRAC 0.95f
#endif

#ifndef BTM_TEMP_MIN_C
#define BTM_TEMP_MIN_C -20.0f
#endif

#ifndef BTM_TEMP_MAX_C
#define BTM_TEMP_MAX_C 80.0f
#endif
