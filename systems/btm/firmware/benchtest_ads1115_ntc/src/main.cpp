#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#include "btm_config.h"
#include "ntc_math.h"

struct ChannelFlags {
  bool sensor_missing = false;
  bool out_of_range = false;
  bool adc_read_fail = false;
};

struct ChannelReading {
  int16_t raw = 0;
  float volts = NAN;
  float r_ohm = NAN;
  float t_c = NAN;
  ChannelFlags flags;
};

static Adafruit_ADS1115 g_ads[BTM_ADS_COUNT];
static bool g_ads_ok[BTM_ADS_COUNT] = {false, false, false, false};

static ChannelReading g_readings[BTM_CHANNEL_COUNT];

static bool i2c_ping(uint8_t addr) {
  Wire.beginTransmission(addr);
  const uint8_t err = Wire.endTransmission();
  return (err == 0);
}

void init_i2c() {
  Wire.begin(BTM_I2C_SDA_PIN, BTM_I2C_SCL_PIN);
  Wire.setClock(BTM_I2C_FREQ_HZ);
}

uint8_t scan_ads_devices() {
  uint8_t found = 0;

  for (uint8_t i = 0; i < BTM_ADS_COUNT; i++) {
    const uint8_t addr = BTM_ADS_ADDRS[i];

    if (!i2c_ping(addr)) {
      g_ads_ok[i] = false;
      continue;
    }

    // Initialize ADS1115 instance on this address.
    if (g_ads[i].begin(addr, &Wire)) {
      g_ads[i].setGain(GAIN_ONE);  // matches default BTM_ADS_FS_VOLTS=4.096V
      // Optional: choose a moderate data rate (bench friendly)
      g_ads[i].setDataRate(RATE_ADS1115_128SPS);
      g_ads_ok[i] = true;
      found++;
    } else {
      g_ads_ok[i] = false;
    }
  }

  return found;
}

bool read_adc_channel(uint8_t device_index, uint8_t channel, int16_t* out_raw) {
  if (!out_raw) return false;
  if (device_index >= BTM_ADS_COUNT) return false;
  if (channel >= BTM_CHANNELS_PER_ADS) return false;
  if (!g_ads_ok[device_index]) return false;

  const uint8_t addr = BTM_ADS_ADDRS[device_index];
  if (!i2c_ping(addr)) return false;

  // Single-ended read (AINx vs GND)
  const int16_t raw = g_ads[device_index].readADC_SingleEnded(channel);
  *out_raw = raw;
  return true;
}

float adc_to_voltage(int16_t raw) {
  // ADS1115 raw is signed 16-bit; in single-ended mode it should be >= 0.
  // Conversion: V = raw * FS / 32768
  return ((float)raw * BTM_ADS_FS_VOLTS) / 32768.0f;
}

bool voltage_to_resistance(float v_out, float vcc, float r_fixed, float* out_r_ntc) {
  if (!out_r_ntc) return false;

  if (!isfinite(v_out) || !isfinite(vcc) || !isfinite(r_fixed)) return false;
  if (vcc <= 0.0f || r_fixed <= 0.0f) return false;

  // Guard against divide-by-zero and non-physical values
  if (v_out <= 0.0f || v_out >= vcc) return false;

  const BtmDividerOrientation orient = (BtmDividerOrientation)BTM_DIVIDER_ORIENTATION;

  float r_ntc = NAN;
  if (orient == BTM_R_FIXED_TOP__NTC_BOTTOM) {
    // Vout = Vcc * (Rntc / (Rfixed + Rntc)) => Rntc = Rfixed * Vout / (Vcc - Vout)
    r_ntc = r_fixed * v_out / (vcc - v_out);
  } else {
    // Vout = Vcc * (Rfixed / (Rfixed + Rntc)) => Rntc = Rfixed * (Vcc - Vout) / Vout
    r_ntc = r_fixed * (vcc - v_out) / v_out;
  }

  if (!isfinite(r_ntc) || r_ntc <= 0.0f) return false;

  *out_r_ntc = r_ntc;
  return true;
}

float resistance_to_temperature(float r_ntc_ohm) {
  return ntc_resistance_to_c(r_ntc_ohm, kBtmDefaultNtc);
}

static void print_config(uint8_t ads_found) {
  Serial.println();
  Serial.println(F("=== BTM benchtest: ADS1115 + NTC ==="));
  Serial.printf("I2C pins: SDA=%d, SCL=%d, freq=%u Hz\n", BTM_I2C_SDA_PIN, BTM_I2C_SCL_PIN, (unsigned)BTM_I2C_FREQ_HZ);
  Serial.printf("Divider: Vcc=%.3f V, Rfixed=%.1f ohm, orientation=%s\n",
                (double)BTM_DIVIDER_VCC_VOLTS,
                (double)BTM_DIVIDER_R_FIXED_OHM,
                (BTM_DIVIDER_ORIENTATION == BTM_R_FIXED_TOP__NTC_BOTTOM) ? "Rfixed-top / NTC-bottom" : "NTC-top / Rfixed-bottom");
  Serial.printf("ADS1115: expected addrs = [0x%02X, 0x%02X, 0x%02X, 0x%02X], found=%u\n",
                BTM_ADS_ADDRS[0], BTM_ADS_ADDRS[1], BTM_ADS_ADDRS[2], BTM_ADS_ADDRS[3], ads_found);
  Serial.printf("ADC scaling: FS=%.3f V => V = raw * FS / 32768\n", (double)BTM_ADS_FS_VOLTS);

  Serial.println(F("NTC: default model = Beta, R25=10k, B=3950 (repo baseline)"));
  Serial.printf("Temp range flags: [%.1f .. %.1f] C\n", (double)BTM_TEMP_MIN_C, (double)BTM_TEMP_MAX_C);
  Serial.println();
}

static void print_table_header() {
  Serial.println(F("idx  addr  ain  raw     V        R_ohm      T_C     sensor_missing out_of_range adc_read_fail"));
  Serial.println(F("---  ----  ---  ------ -------- ---------- -------  ------------- ------------ ------------"));
}

void print_table() {
  for (uint8_t idx = 0; idx < BTM_CHANNEL_COUNT; idx++) {
    const uint8_t dev = idx / BTM_CHANNELS_PER_ADS;
    const uint8_t ch  = idx % BTM_CHANNELS_PER_ADS;
    const uint8_t addr = BTM_ADS_ADDRS[dev];
    const ChannelReading& r = g_readings[idx];

    Serial.printf("%02u   0x%02X  AIN%u %6d  %7.3f  %10.1f  %7.2f  %13u %12u %12u\n",
                  idx, addr, ch,
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

static void sample_all_channels() {
  const float vcc = BTM_DIVIDER_VCC_VOLTS;
  const float r_fixed = BTM_DIVIDER_R_FIXED_OHM;

  const float v_short = vcc * BTM_SENSOR_SHORT_FRAC;
  const float v_open  = vcc * BTM_SENSOR_OPEN_FRAC;

  for (uint8_t idx = 0; idx < BTM_CHANNEL_COUNT; idx++) {
    const uint8_t dev = idx / BTM_CHANNELS_PER_ADS;
    const uint8_t ch  = idx % BTM_CHANNELS_PER_ADS;

    ChannelReading out;
    int16_t raw = 0;

    const bool ok = read_adc_channel(dev, ch, &raw);
    out.raw = raw;

    if (!ok) {
      out.flags.adc_read_fail = true;
      g_readings[idx] = out;
      continue;
    }

    const float v = adc_to_voltage(raw);
    out.volts = v;

    // sensor_missing: close to rails looks like open/short or wiring fault
    if (!isfinite(v) || v <= v_short || v >= v_open) {
      out.flags.sensor_missing = true;
      g_readings[idx] = out;
      continue;
    }

    float r_ntc = NAN;
    if (!voltage_to_resistance(v, vcc, r_fixed, &r_ntc)) {
      out.flags.sensor_missing = true; // can't compute resistance => treat as wiring issue for bench
      g_readings[idx] = out;
      continue;
    }

    out.r_ohm = r_ntc;

    const float t_c = resistance_to_temperature(r_ntc);
    out.t_c = t_c;

    if (!isfinite(t_c) || t_c < BTM_TEMP_MIN_C || t_c > BTM_TEMP_MAX_C) {
      out.flags.out_of_range = true;
    }

    g_readings[idx] = out;
  }
}

void setup() {
  Serial.begin(BTM_SERIAL_BAUD);
  delay(200);

  init_i2c();

  const uint8_t ads_found = scan_ads_devices();
  print_config(ads_found);
  print_table_header();
}

void loop() {
  static uint32_t last_ms = 0;
  const uint32_t now = millis();

  if (now - last_ms >= BTM_PRINT_PERIOD_MS) {
    last_ms = now;
    sample_all_channels();
    print_table();
  }
}
