#pragma once
#include <Arduino.h>
#include <math.h>

// ============================================================
// NTC math helpers for bench test
//
// Default model: Beta equation (good enough for sanity checks).
// Optional: Steinhart–Hart (A/B/C) if you later paste coefficients from datasheet.
// ============================================================

struct NtcParams {
  float r0_ohm;      // nominal resistance at T0 (e.g., 10k @ 25C)
  float t0_c;        // nominal temperature (usually 25C)
  float beta_k;      // Beta value in Kelvin (e.g., 3950)

  // Optional Steinhart–Hart coefficients (example placeholders)
  float sh_a;
  float sh_b;
  float sh_c;
  bool use_steinhart;
};

static constexpr float kKelvinOffset = 273.15f;

// Repo baseline indicates B3950 + 10k NTC, so defaults match that.
static constexpr NtcParams kBtmDefaultNtc = {
  10000.0f,     // r0_ohm
  25.0f,        // t0_c
  3950.0f,      // beta_k
  // Example SH coefficients (NOT guaranteed for your exact thermistor!)
  // Replace with datasheet values if you want Steinhart–Hart accuracy.
  0.001129148f, // sh_a
  0.000234125f, // sh_b
  0.0000000876741f, // sh_c
  false         // use_steinhart
};

inline float ntc_resistance_to_c_beta(float r_ohm, const NtcParams& p) {
  if (!isfinite(r_ohm) || r_ohm <= 0.0f) return NAN;

  const float t0_k = p.t0_c + kKelvinOffset;
  const float inv_t = (1.0f / t0_k) + (1.0f / p.beta_k) * logf(r_ohm / p.r0_ohm);
  if (!isfinite(inv_t) || inv_t <= 0.0f) return NAN;

  const float t_k = 1.0f / inv_t;
  return t_k - kKelvinOffset;
}

// Steinhart–Hart: T(K) = 1 / (A + B ln(R) + C (ln(R))^3)
inline float ntc_resistance_to_c_steinhart(float r_ohm, const NtcParams& p) {
  if (!isfinite(r_ohm) || r_ohm <= 0.0f) return NAN;

  const float ln_r = logf(r_ohm);
  const float denom = p.sh_a + (p.sh_b * ln_r) + (p.sh_c * ln_r * ln_r * ln_r);
  if (!isfinite(denom) || denom <= 0.0f) return NAN;

  const float t_k = 1.0f / denom;
  return t_k - kKelvinOffset;
}

inline float ntc_resistance_to_c(float r_ohm, const NtcParams& p) {
  if (p.use_steinhart) return ntc_resistance_to_c_steinhart(r_ohm, p);
  return ntc_resistance_to_c_beta(r_ohm, p);
}

// Small reference table for sanity checks (b3950, r25=10k).
// Values are approximate; useful for spotting totally wrong conversions.
struct NtcTablePoint { float t_c; float r_ohm; };

static constexpr NtcTablePoint kBtmExampleTable[] = {
  {-20.0f, 105384.7f},
  {  0.0f,  33620.6f},
  { 25.0f,  10000.0f},
  { 40.0f,   5301.5f},
  { 60.0f,   2486.2f},
  { 80.0f,   1270.3f}
};
