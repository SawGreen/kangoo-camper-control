# BTM Bench Test Plan (Tomorrow Bring-Up)

## 1) Purpose

This bench test verifies the basic measurement chain only:
- ESP32 I2C bring-up
- 4x ADS1115 detection
- 16x channel sampling
- raw ADC to voltage
- voltage to NTC resistance
- resistance to temperature (Beta model)
- readable Serial output with simple flags

This is a practical bench test, not production integration.

## 2) In Scope

Bench firmware should do only this:
- Initialize Serial and I2C.
- Detect ADS1115 devices at configured addresses.
- Read all 16 single-ended channels.
- Convert `raw -> volts -> ohms -> degrees C`.
- Print one readable table to Serial.
- Set per-channel flags:
  - `sensor_missing` (simple bench fault flag for missing sensor behavior, open/short-like divider faults, and near-rail readings)
  - `out_of_range`
  - `adc_read_fail`

## 3) Out of Scope

Explicitly not part of this task:
- Production architecture changes
- Wi-Fi/network features
- Webserver/GUI/UI
- Persistent config/calibration storage
- Message bus or MCS integration
- Final system integration patterns

## 4) Minimum Hardware

Required:
- 1x ESP32-WROOM-32 class dev board (USB-C, CH340C)
- 4x generic ADS1115 modules
- 16x NTC 10k B3950
- 16x fixed resistor 10k (1% recommended)
- Jumpers, breadboard/terminal block
- USB data cable (not charge-only)

Recommended:
- Multimeter
- Labels for channel IDs
- Mild heat source for one-sensor response check

## 5) Practical Step-by-Step Procedure

### Step A: Wiring Check
1. Wire per `systems/btm/hardware/wiring-notes.md`.
2. Confirm shared GND between ESP32 and all ADS modules.
3. Confirm unique ADS addresses: `0x48, 0x49, 0x4A, 0x4B`.
4. Confirm divider orientation in hardware matches firmware config.

### Step B: IDE + Upload
1. Follow `systems/btm/tests/bench-setup-requirements.md`.
2. Select board `ESP32 Dev Module`.
3. Upload firmware and open Serial Monitor at `115200`.

### Step C: Startup Checks
1. Confirm startup prints I2C configuration.
2. Confirm startup prints expected ADS addresses and detected count.
3. If any ADS is missing, stop and fix wiring/address straps first.

### Step D: Readout Checks
1. Wait for table output.
2. Confirm all global channels 0..15 are listed.
3. Confirm channels on missing ADS devices show `adc_read_fail=1`.

### Step E: Sanity Temperature Check
1. At room conditions, verify values are plausible.
2. Warm exactly one sensor.
3. Confirm that channel changes clearly and others stay mostly stable.

### Step F: Fault Flag Check
1. Disconnect one sensor path; confirm `sensor_missing=1`.
2. Create known invalid condition (near rail input) and confirm missing/fault behavior.
3. Push one sensor outside temp limits (if practical) to confirm `out_of_range=1`.

### Step G: Log the Session
1. Fill Pack Build Log.
2. Fill BMS Comparison Log (if BMS/reference readings exist).
3. Fill BTM Bench Log with channel-level observations.

## 6) Pass/Fail Criteria

PASS if all are true:
- Firmware starts and prints config info.
- Exactly four ADS1115 devices are detected.
- 16 channels print repeatedly.
- Conversion values are plausible for bench conditions.
- Flags react as expected during basic fault checks.

FAIL if any are true:
- I2C does not start reliably.
- ADS count is not 4.
- Channel data cannot be read repeatedly.
- Values are clearly non-physical for normal bench temperature.
- Flags do not react to simple injected faults.

## 7) Manual Logging Templates

## Pack Build Log
- Date:
- Operator:
- Board serial/port:
- ADS module labels:
- Divider orientation:
- Sensor label map used:
- Wiring notes/deviations:
- Issues found:
- Fix applied:

## BMS Comparison Log
- Date:
- BMS / reference instrument:

| Group/Cell ID | BMS Voltage (V) | Bench Voltage (V) | Delta (mV) | Notes |
|---|---:|---:|---:|---|
|   |   |   |   |   |
|   |   |   |   |   |
|   |   |   |   |   |

## BTM Bench Log
- Date:
- Firmware commit:
- Ambient estimate (C):
- Firmware config snapshot:
  - Divider orientation:
  - Beta:
  - R25 (ohm):
  - Temperature limits (C):

| ADS Addr | AIN | CH | Raw | Voltage (V) | NTC (ohm) | Temp (C) | sensor_missing | out_of_range | adc_read_fail | Notes |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0x48 | AIN0 | 0  |  |  |  |  |  |  |  |  |
| 0x48 | AIN1 | 1  |  |  |  |  |  |  |  |  |
| 0x48 | AIN2 | 2  |  |  |  |  |  |  |  |  |
| 0x48 | AIN3 | 3  |  |  |  |  |  |  |  |  |
| 0x49 | AIN0 | 4  |  |  |  |  |  |  |  |  |
| 0x49 | AIN1 | 5  |  |  |  |  |  |  |  |  |
| 0x49 | AIN2 | 6  |  |  |  |  |  |  |  |  |
| 0x49 | AIN3 | 7  |  |  |  |  |  |  |  |  |
| 0x4A | AIN0 | 8  |  |  |  |  |  |  |  |  |
| 0x4A | AIN1 | 9  |  |  |  |  |  |  |  |  |
| 0x4A | AIN2 | 10 |  |  |  |  |  |  |  |  |
| 0x4A | AIN3 | 11 |  |  |  |  |  |  |  |  |
| 0x4B | AIN0 | 12 |  |  |  |  |  |  |  |  |
| 0x4B | AIN1 | 13 |  |  |  |  |  |  |  |  |
| 0x4B | AIN2 | 14 |  |  |  |  |  |  |  |  |
| 0x4B | AIN3 | 15 |  |  |  |  |  |  |  |  |
