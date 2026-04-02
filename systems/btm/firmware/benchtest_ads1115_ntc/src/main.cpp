#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#include "btm_config.h"
#include "ntc_math.h"

struct ChannelFlags {
  bool sensor_missing;
  bool out_of_range;
  bool adc_read_fail;
};

struct ChannelReading {
  int16_t raw;
  float volts;
  float r_ohm;
  float t_c;
  ChannelFlags flags;
};

static Adafruit_ADS1115 g_ads[BTM_ADS_COUNT];
static bool g_ads_ok[BTM_ADS_COUNT] = {false, false, false, false};
static ChannelReading g_readings[BTM_CHANNEL_COUNT];
static uint32_t g_cycle_count = 0;

static float adc_to_voltage(int16_t raw) {
  return ((float)raw * BTM_ADS_FS_VOLTS) / 32768.0f;
}

static bool voltage_to_ntc_resistance(float v_out, float* out_r_ntc) {
  if (!out_r_ntc) return false;
  if (!isfinite(v_out)) return false;

  const float vcc = BTM_DIVIDER_VCC_VOLTS;
  const float r_fixed = BTM_DIVIDER_R_FIXED_OHM;
  if (vcc <= 0.0f || r_fixed <= 0.0f) return false;

  if (v_out <= 0.0f || v_out >= vcc) return false;

  if (BTM_DIVIDER_ORIENTATION == BTM_R_FIXED_TOP__NTC_BOTTOM) {
    *out_r_ntc = r_fixed * v_out / (vcc - v_out);
  } else {
    *out_r_ntc = r_fixed * (vcc - v_out) / v_out;
  }

  return isfinite(*out_r_ntc) && (*out_r_ntc > 0.0f);
}

static void init_i2c() {
  Wire.begin(BTM_I2C_SDA_PIN, BTM_I2C_SCL_PIN);
  Wire.setClock(BTM_I2C_FREQ_HZ);
}

static uint8_t begin_ads_devices() {
  uint8_t found = 0;

  for (uint8_t i = 0; i < BTM_ADS_COUNT; i++) {
    const uint8_t addr = BTM_ADS_ADDRS[i];

    if (g_ads[i].begin(addr, &Wire)) {
      g_ads[i].setGain(GAIN_ONE);
      g_ads[i].setDataRate(RATE_ADS1115_128SPS);
      g_ads_ok[i] = true;
      found++;
    } else {
      g_ads_ok[i] = false;
    }
  }

  return found;
}

static void print_config(uint8_t ads_found) {
  Serial.println();
  Serial.println(F("=== BTM BENCH TEST ==="));
  Serial.printf("Board target: ESP32 Dev Module\n");
  Serial.printf("I2C: SDA=%d SCL=%d FREQ=%u Hz\n", BTM_I2C_SDA_PIN, BTM_I2C_SCL_PIN, (unsigned)BTM_I2C_FREQ_HZ);
  Serial.printf("ADS expected: 0x%02X 0x%02X 0x%02X 0x%02X | found=%u\n",
                BTM_ADS_ADDRS[0], BTM_ADS_ADDRS[1], BTM_ADS_ADDRS[2], BTM_ADS_ADDRS[3], ads_found);
  Serial.printf("Divider: VCC=%.3fV R_FIXED=%.1fohm\n", (double)BTM_DIVIDER_VCC_VOLTS, (double)BTM_DIVIDER_R_FIXED_OHM);
  Serial.printf("Orientation: %s\n",
                (BTM_DIVIDER_ORIENTATION == BTM_R_FIXED_TOP__NTC_BOTTOM) ? "Rfixed_to_VCC / NTC_to_GND" : "NTC_to_VCC / Rfixed_to_GND");
  Serial.printf("NTC Beta model: Beta=%.1f R25=%.1fohm T0=%.1fC\n",
                (double)BTM_NTC_BETA_K, (double)BTM_NTC_R25_OHM, (double)BTM_NTC_T0_C);
  Serial.printf("Temperature flag range: %.1fC .. %.1fC\n", (double)BTM_TEMP_MIN_C, (double)BTM_TEMP_MAX_C);
  Serial.println();
}

static void print_table_header() {
  Serial.println(F("idx  addr  ain  raw     volts    r_ohm      temp_c   sensor_missing out_of_range adc_read_fail"));
  Serial.println(F("---  ----  ---  ------  -------  ---------  -------  ------------- ------------ ------------"));
}

static void sample_all_channels() {
  const float vcc = BTM_DIVIDER_VCC_VOLTS;
  const float v_short_limit = vcc * BTM_SENSOR_SHORT_FRAC;
  const float v_open_limit = vcc * BTM_SENSOR_OPEN_FRAC;

  for (uint8_t idx = 0; idx < BTM_CHANNEL_COUNT; idx++) {
    ChannelReading out = {0, NAN, NAN, NAN, {false, false, false}};

    const uint8_t dev = idx / BTM_CHANNELS_PER_ADS;
    const uint8_t ain = idx % BTM_CHANNELS_PER_ADS;

    if (!g_ads_ok[dev]) {
      out.flags.adc_read_fail = true;
      g_readings[idx] = out;
      continue;
    }

    out.raw = g_ads[dev].readADC_SingleEnded(ain);

    // In single-ended mode, negative raw usually means invalid reading on this setup.
    if (out.raw < 0) {
      out.flags.adc_read_fail = true;
      g_readings[idx] = out;
      continue;
    }

    out.volts = adc_to_voltage(out.raw);

    if (!isfinite(out.volts) || out.volts <= v_short_limit || out.volts >= v_open_limit) {
      out.flags.sensor_missing = true;
      g_readings[idx] = out;
      continue;
    }

    if (!voltage_to_ntc_resistance(out.volts, &out.r_ohm)) {
      out.flags.sensor_missing = true;
      g_readings[idx] = out;
      continue;
    }

    out.t_c = ntc_resistance_to_c_beta(out.r_ohm, BTM_NTC_BETA_K, BTM_NTC_R25_OHM, BTM_NTC_T0_C);

    if (!isfinite(out.t_c) || out.t_c < BTM_TEMP_MIN_C || out.t_c > BTM_TEMP_MAX_C) {
      out.flags.out_of_range = true;
    }

    g_readings[idx] = out;
  }
}

static void print_table() {
  g_cycle_count++;
  Serial.printf("cycle=%lu uptime_ms=%lu\n", (unsigned long)g_cycle_count, (unsigned long)millis());

  // Reprint header periodically to keep long serial logs readable.
  if ((g_cycle_count % 10U) == 1U) {
    print_table_header();
  }

  for (uint8_t idx = 0; idx < BTM_CHANNEL_COUNT; idx++) {
    const uint8_t dev = idx / BTM_CHANNELS_PER_ADS;
    const uint8_t ain = idx % BTM_CHANNELS_PER_ADS;
    const uint8_t addr = BTM_ADS_ADDRS[dev];
    const ChannelReading& r = g_readings[idx];

    Serial.printf("%02u   0x%02X  AIN%u %6d  %7.3f  %9.1f  %7.2f  %13u %12u %12u\n",
                  idx,
                  addr,
                  ain,
                  (int)r.raw,
                  (double)r.volts,
                  (double)r.r_ohm,
                  (double)r.t_c,
                  r.flags.sensor_missing ? 1 : 0,
                  r.flags.out_of_range ? 1 : 0,
                  r.flags.adc_read_fail ? 1 : 0);
  }
  Serial.println();
}

void setup() {
  Serial.begin(BTM_SERIAL_BAUD);
  delay(300);

  init_i2c();
  const uint8_t ads_found = begin_ads_devices();

  print_config(ads_found);
  print_table_header();
}

void loop() {
  static uint32_t last_print_ms = 0;
  const uint32_t now = millis();

  if (now - last_print_ms >= BTM_PRINT_PERIOD_MS) {
    last_print_ms = now;
    sample_all_channels();
    print_table();
  }
}
