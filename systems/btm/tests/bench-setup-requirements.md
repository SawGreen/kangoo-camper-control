# BTM Bench Setup Requirements (Arduino IDE 2.x)

This setup note is written for first bring-up tomorrow with:
- ESP32-WROOM-32 class dev board
- USB-C connector
- CH340C USB-UART bridge
- Arduino board target: `ESP32 Dev Module`

PlatformIO files are included in the repo. Arduino IDE is a practical bench fallback workflow for tomorrow if PlatformIO is not available on the test laptop.

## 1) Install Arduino IDE 2.x

- Use Arduino IDE 2.x.
- Confirm Boards Manager and Library Manager are available.

## 2) Install `esp32 by Espressif Systems`

1. Open **File -> Preferences**.
2. In **Additional boards manager URLs**, add:
   - `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Open **Tools -> Board -> Boards Manager**.
4. Search for `esp32 by Espressif Systems`.
5. Install it.

## 3) Select the Board

- Open **Tools -> Board -> esp32**.
- Select **ESP32 Dev Module**.

## 4) Install ADS Library

1. Open **Tools -> Manage Libraries**.
2. Search `Adafruit ADS1X15`.
3. Install `Adafruit ADS1X15`.

Note:
- `Wire` is part of the Arduino ESP32 core and does not need separate installation.

## 5) Use the Repo Firmware in Arduino IDE

Firmware source files are here:
- `systems/btm/firmware/benchtest_ads1115_ntc/src/main.cpp`
- `systems/btm/firmware/benchtest_ads1115_ntc/include/btm_config.h`
- `systems/btm/firmware/benchtest_ads1115_ntc/include/ntc_math.h`

Practical Arduino IDE method:
1. Create a new sketch folder named `benchtest_ads1115_ntc`.
2. Copy `main.cpp` into the sketch and rename it to `benchtest_ads1115_ntc.ino`.
3. Add two tabs/files in the sketch: `btm_config.h` and `ntc_math.h`.
4. Paste the header contents from the repo files.
5. Save and compile.

## 6) USB-C Connection and Port Selection

1. Connect the ESP32 board using a **USB data cable** (not charge-only).
2. Wait for OS USB enumeration.
3. In Arduino IDE, open **Tools -> Port**.
4. Select the port for the CH340C device.

## 7) Serial Monitor Settings

- Set baud rate to `115200`.
- This matches firmware `Serial.begin(115200)`.

## 8) Short First-Upload Procedure

1. Confirm board is `ESP32 Dev Module`.
2. Confirm correct serial/COM port is selected.
3. Click **Upload**.
4. Open Serial Monitor at `115200`.
5. Confirm startup prints:
   - I2C pin/frequency line
   - ADS expected addresses line
   - ADS found count

Example startup output (format may vary slightly):
```text
=== BTM BENCH TEST ===
Board target: ESP32 Dev Module
I2C: SDA=21 SCL=22 FREQ=100000 Hz
ADS expected: 0x48 0x49 0x4A 0x4B | found=4
Divider: VCC=3.300V R_FIXED=10000.0ohm
```

## 9) Common Upload Problems

- **Upload timeout / failed to connect**
  - Re-check board and selected port.
  - Press and hold **BOOT** while upload starts, then release when writing begins.
  - Re-plug USB cable and retry.
- **Compilation fails due to missing library**
  - Confirm `Adafruit ADS1X15` is installed.
- **Board not detected**
  - Replace cable with known data cable.
  - Try another USB port.

## 10) Common Serial Detection Problems (CH340C)

If board/port does not appear, CH340 driver troubleshooting may be required:
- Verify CH340/CH34x driver installation for your OS.
- Reboot after driver install if needed.
- Check OS device manager/system USB list for CH340 device presence.

If still not detected:
- Test with another USB cable.
- Test with another USB port.
- Test the board on another computer.
