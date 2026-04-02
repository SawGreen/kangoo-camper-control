#pragma once
#include <stdint.h>

// ============================================================
// Bench configuration (edit for your exact bench wiring)
//
// Repo explicitly keeps some topics open (ESP32 variant, calibration model, wiring).
// This header makes those bench-critical knobs explicit and local.
// ============================================================

// ---------- Serial ----------
#ifndef BTM_SERIAL_BAUD
#define BTM_SERIAL_BAUD 115200
#endif

#ifndef BTM_PRINT_PERIOD_MS
#define BTM_PRINT_PERIOD_MS 500
#endif

// ---------- I2C (ESP32 pins) ----------
// Repo does NOT specify SDA/SCL pins. Defaults match common ESP32 DevKit wiring.
#ifndef BTM_I2C_SDA_PIN
#define BTM_I2C_SDA_PIN 21
#endif

#ifndef BTM_I2C_SCL_PIN
#define BTM_I2C_SCL_PIN 22
#endif

#ifndef BTM_I2C_FREQ_HZ
#define BTM_I2C_FREQ_HZ 400000
#endif

// ---------- ADS1115 device set ----------
static constexpr uint8_t BTM_ADS_COUNT = 4;
static constexpr uint8_t BTM_CHANNELS_PER_ADS = 4;
static constexpr uint8_t BTM_CHANNEL_COUNT = BTM_ADS_COUNT * BTM_CHANNELS_PER_ADS;

// Expected addresses for 4x ADS1115 with ADDR strapped to GND/VDD/SDA/SCL.
static constexpr uint8_t BTM_ADS_ADDRS[BTM_ADS_COUNT] = {0x48, 0x49, 0x4A, 0x4B};

// ---------- ADC conversion (raw -> volts) ----------
// Bench assumption: ADS1115 PGA set to GAIN_ONE (FS ±4.096 V).
// raw_to_volts = raw * FS / 32768.
#ifndef BTM_ADS_FS_VOLTS
#define BTM_ADS_FS_VOLTS 4.096f
#endif

// ---------- Divider parameters ----------
// Bench assumption: divider powered from 3.3 V (same as ADS VDD recommended).
#ifndef BTM_DIVIDER_VCC_VOLTS
#define BTM_DIVIDER_VCC_VOLTS 3.300f
#endif

// Repo baseline resistor: 10k 1% (fixed leg for each NTC divider).
#ifndef BTM_DIVIDER_R_FIXED_OHM
#define BTM_DIVIDER_R_FIXED_OHM 10000.0f
#endif

// Divider orientation selector
enum BtmDividerOrientation : uint8_t {
  BTM_R_FIXED_TOP__NTC_BOTTOM = 0,  // Vcc--Rfixed--+--NTC--GND, Vout at '+'
  BTM_NTC_TOP__R_FIXED_BOTTOM = 1   // Vcc--NTC--+--Rfixed--GND, Vout at '+'
};

// Choose ONE and keep wiring consistent.
// If you change the bench wiring, change this value accordingly.
#ifndef BTM_DIVIDER_ORIENTATION
#define BTM_DIVIDER_ORIENTATION BTM_R_FIXED_TOP__NTC_BOTTOM
#endif

// ---------- Flags / plausibility thresholds ----------
#ifndef BTM_SENSOR_SHORT_FRAC
#define BTM_SENSOR_SHORT_FRAC 0.05f  // Vout < 5% Vcc => looks like short / hard fault
#endif

#ifndef BTM_SENSOR_OPEN_FRAC
#define BTM_SENSOR_OPEN_FRAC 0.95f   // Vout > 95% Vcc => looks like open / missing
#endif

#ifndef BTM_TEMP_MIN_C
#define BTM_TEMP_MIN_C -20.0f
#endif

#ifndef BTM_TEMP_MAX_C
#define BTM_TEMP_MAX_C 80.0f
#endif
