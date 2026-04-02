# BTM Wiring Notes (Bench-Focused)

This note is only for practical bench wiring with:
- 1x ESP32-WROOM-32 class dev board (USB-C, CH340C)
- 4x generic ADS1115 modules
- 16x 10k B3950 NTC sensors
- 16x fixed 10k resistors

## 1) ESP32 I2C Defaults for Bench Bring-Up

For Arduino target `ESP32 Dev Module`, use:
- `SDA = GPIO21`
- `SCL = GPIO22`

Recommended first bring-up I2C frequency:
- `100000 Hz` (conservative and usually stable)

After basic success, `400000 Hz` can be tried if wiring is short and clean.

## 2) ADS1115 Address Selection

ADS1115 address depends on ADDR pin strap:
- `ADDR -> GND` = `0x48`
- `ADDR -> VDD` = `0x49`
- `ADDR -> SDA` = `0x4A`
- `ADDR -> SCL` = `0x4B`

## 3) Recommended 4-Device Address Plan

Use one module per address:
- ADS #0: `0x48`
- ADS #1: `0x49`
- ADS #2: `0x4A`
- ADS #3: `0x4B`

This matches the firmware defaults.

## 4) Power Assumptions (Conservative)

For this bench setup, keep all logic at 3.3 V:
- ESP32 logic: 3.3 V
- ADS1115 supply: 3.3 V
- Divider supply: 3.3 V

Also required:
- Common GND between ESP32 and all ADS modules.

Conservative warning:
- Some generic ADS1115 boards support 5 V power, but that may affect logic-level assumptions depending on module design.
- For tomorrow, use 3.3 V everywhere unless you have verified your exact module behavior.

## 5) NTC Divider Concept

Each channel is a 2-resistor divider measured by ADS1115:
- One leg is NTC.
- One leg is fixed 10k.
- ADS reads `Vout` at the midpoint.

Firmware then computes:
- `ADC raw -> voltage -> NTC resistance -> temperature`.

## 6) Valid Divider Orientations and Formulas

Definitions:
- `Vcc` = divider supply
- `Vout` = midpoint voltage
- `Rfixed` = fixed resistor
- `Rntc` = NTC resistance

### Orientation A: NTC to VCC, fixed resistor to GND

`VCC -- NTC -- Vout -- Rfixed -- GND`

Use:
- `Rntc = Rfixed * (Vcc - Vout) / Vout`

### Orientation B: fixed resistor to VCC, NTC to GND

`VCC -- Rfixed -- Vout -- NTC -- GND`

Use:
- `Rntc = Rfixed * Vout / (Vcc - Vout)`

Both are valid. Firmware orientation must match actual wiring.

## 7) Channel Mapping for All 16 Inputs

Use this global map:
- ADS `0x48`: AIN0->CH0, AIN1->CH1, AIN2->CH2, AIN3->CH3
- ADS `0x49`: AIN0->CH4, AIN1->CH5, AIN2->CH6, AIN3->CH7
- ADS `0x4A`: AIN0->CH8, AIN1->CH9, AIN2->CH10, AIN3->CH11
- ADS `0x4B`: AIN0->CH12, AIN1->CH13, AIN2->CH14, AIN3->CH15

Label this map on the bench harness to avoid channel confusion.

## 8) Why ADS Boards Should Be Close to Sensors

For this type of NTC divider measurement, short analog paths are helpful:
- Less noise pickup
- Less ground-offset error
- Easier troubleshooting of unstable readings

Practical rule for tomorrow:
- Keep divider/ADC wiring short.
- Allow longer distance on digital lines (I2C) only if needed.
