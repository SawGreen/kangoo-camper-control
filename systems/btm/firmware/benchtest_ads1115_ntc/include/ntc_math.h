#pragma once
#include <math.h>

// Minimal bench math helpers for a 10k NTC using the Beta model.
// This is intentionally simple and easy to inspect.

static constexpr float BTM_KELVIN_OFFSET = 273.15f;

inline float ntc_resistance_to_c_beta(float r_ntc_ohm, float beta_k, float r0_ohm, float t0_c) {
  if (!isfinite(r_ntc_ohm) || !isfinite(beta_k) || !isfinite(r0_ohm) || !isfinite(t0_c)) return NAN;
  if (r_ntc_ohm <= 0.0f || beta_k <= 0.0f || r0_ohm <= 0.0f) return NAN;

  const float t0_k = t0_c + BTM_KELVIN_OFFSET;
  if (t0_k <= 0.0f) return NAN;

  const float inv_t = (1.0f / t0_k) + (1.0f / beta_k) * logf(r_ntc_ohm / r0_ohm);
  if (!isfinite(inv_t) || inv_t <= 0.0f) return NAN;

  const float t_k = 1.0f / inv_t;
  return t_k - BTM_KELVIN_OFFSET;
}
