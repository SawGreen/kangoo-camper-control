# ADR-001: Battery Temperature Monitoring Architecture for 4P4S LiFePO4 Pack

- **Status:** Accepted
- **Date:** 2026-04-03
- **Deciders:** SawGreen
- **Consulted:** ChatGPT
- **Informed:** N/A

---

## Context

The battery pack for the camper project is being built from reclaimed **LiFePO4 32650 cells** originally taken from unused model battery packs.

The final battery topology is:

- **16 cells total**
- **4P4S configuration**
- 4 parallel groups
- 4 series groups

Because the battery is being assembled from reclaimed cells rather than from a factory-matched integrated pack, additional thermal visibility was considered necessary.

The selected JK BMS provides electrical battery telemetry and protection functions, but the project requires **additional independent temperature monitoring** beyond the BMS.

The main reasons are:

- increased confidence when using reclaimed cells
- improved thermal visibility inside the pack
- ability to detect abnormal thermal behavior
- ability to generate custom warnings and future control logic independent of BMS internal limits
- better support for diagnostics, testing, and long-term monitoring

This led to the decision to create a dedicated **BTM (Battery Temperature Monitoring)** subsystem.

---

## Decision

The battery temperature monitoring subsystem will use the following **initial hardware baseline for bench bring-up**:

- **1x ESP32-WROOM-32 class development board**
  - USB-C connector
  - CH340C USB-UART bridge
  - initial Arduino IDE board target: `ESP32 Dev Module`
- **4x ADS1115** I2C analog-to-digital converter modules
- **16x B3950-10K-L25MM NTC thermistors**
- **16x 10 kOhm 1% 0.4 W resistors**

The subsystem will measure **temperature at 16 pack positions**, resulting in **16 independent temperature channels**.

The current bench baseline uses:

- **4 ADS1115 devices**
- **4 single-ended inputs per device**
- **16 total analog channels**
- default ADS1115 addresses:
  - `0x48`
  - `0x49`
  - `0x4A`
  - `0x4B`
- default ESP32 I2C pins:
  - `SDA = GPIO21`
  - `SCL = GPIO22`
- conservative initial I2C frequency:
  - `100 kHz`
- common initial supply assumption:
  - **3.3 V logic and divider domain**

The BTM subsystem is therefore accepted as a dedicated system block responsible for:

- raw sensor acquisition
- ADC-to-temperature conversion
- basic sensor plausibility checks
- simple bench fault indication
- future expansion toward filtering, warning logic, and communication to the wider system

---

## Alternatives Considered

### Option A — Use only JK BMS internal temperature monitoring
**Description:**  
Rely only on the temperature sensing and protection functions already available in the JK BMS.

**Advantages:**
- lowest hardware complexity
- no extra node needed
- lower cost
- less wiring

**Disadvantages:**
- insufficient thermal granularity for this pack concept
- limited confidence when using reclaimed cells
- reduced diagnostic capability
- less flexibility for custom warning logic
- dependent on internal BMS measurement strategy and available sensor count

---

### Option B — Dedicated BTM with 16 analog channels using ESP32 + ADS1115 + NTC
**Description:**  
Build an independent temperature monitoring node capable of measuring all 16 pack positions.

**Advantages:**
- full thermal visibility across the monitored pack positions
- independence from BMS thermal sensing
- better diagnostics during pack development and validation
- supports custom warnings and future logic
- scalable and modular subsystem design

**Disadvantages:**
- increased hardware count
- increased wiring complexity
- analog measurement chain requires careful handling of divider assumptions, noise, and plausibility checks
- requires dedicated firmware and integration work

---

### Option C — Use digital temperature sensors instead of NTC + ADC
**Description:**  
Use digital sensors such as 1-Wire or I2C temperature sensors for each monitored position.

**Advantages:**
- less analog frontend design effort
- no ADC scaling network needed
- easier raw digital readout

**Disadvantages:**
- more complex bus behavior with many sensors
- potentially worse packaging/mechanical attachment options
- potentially slower or less robust practical implementation in this physical pack
- higher implementation uncertainty for first version

---

## Rationale

Option B was selected because it offers the best balance between visibility, flexibility, and practical implementation for this battery pack.

The most important decision drivers were:

1. **Thermal visibility across the pack**
2. **Independent monitoring outside the BMS**
3. **Suitability for reclaimed-cell pack architecture**
4. **Diagnostic value during development**
5. **Future expandability**

The pack is not a standard drop-in commercial battery module. It is a custom-built pack from reused cells, which increases the importance of observing thermal behavior more closely than in a conventional integrated battery pack.

Using a dedicated ESP32-based node with 4 ADS1115 devices provides a practical path to 16 measurement channels without introducing more complex analog multiplexing in the first version.

NTC thermistors with matching resistor dividers were selected as a practical, available, and cost-effective approach for distributed temperature sensing inside the battery pack.

The current bench hardware baseline also reflects first bring-up practicality:

- easy USB connection to a laptop
- easy upload through Arduino IDE
- easy Serial-based inspection
- conservative 3.3 V logic assumptions
- simple addressable I2C expansion

This is intentionally a **bench-first architecture baseline**, not yet a final production hardware design.

---

## Consequences

### Positive
- 16 independent temperature channels are available for bench validation
- improved trust in pack operation
- better ability to detect abnormal local heating
- BTM becomes a reusable and testable standalone subsystem
- thermal data can later be integrated into HMI, VCU warnings, and logging

### Negative
- more hardware to buy, wire, mount, and validate
- more firmware complexity than BMS-only monitoring
- analog measurements depend on:
  - correct divider orientation
  - verified thermistor type
  - resistor tolerance
  - clean wiring
  - stable grounding
- I2C and analog routing must be designed carefully
- mechanical attachment of 16 sensors becomes an important design task

### Neutral
- BTM is now a first-class subsystem and must be documented, tested, and versioned like the others
- integration with MCS and VCU remains a later topic
- the current bench baseline should not be mistaken for the final production hardware architecture

---

## Implementation Notes

### Functional scope for the current bench phase
The current bench-oriented BTM implementation is expected to provide at minimum:

- I2C bring-up
- detection of all 4 ADS1115 devices
- reading of all 16 channels
- conversion from ADC raw value to voltage
- conversion from voltage to NTC resistance
- conversion from resistance to temperature
- simple fault flags such as:
  - sensor missing / divider invalid behavior
  - out of range
  - ADC read fail
- readable Serial output for manual inspection

### Hardware scope for the current bench phase
Initial hardware baseline:

- ESP32-WROOM-32 class development board
- USB-C
- CH340C USB-UART bridge
- 4x ADS1115
- 16x B3950-10K-L25MM NTC
- 16x 10k 1% 0.4 W resistor

### Electrical baseline assumptions for bench bring-up
Initial practical assumptions:

- all logic and divider supply kept at **3.3 V**
- common ground between ESP32 and all ADS1115 modules
- divider orientation must be explicitly documented and matched in firmware
- generic ADS1115 modules must be treated carefully because board-level implementation details may vary

### Open implementation topics
This ADR does **not** finalize:

- final production ESP32 module choice
- PCB vs modular board layout
- exact thermistor mounting method on cells
- cable harness design
- shielding / analog grounding strategy
- final calibration workflow
- final temperature conversion model beyond current bench assumptions
- final communication interface from BTM to VCU
- final ownership boundary between BTM preprocessing and VCU logic

Those items will be handled in follow-up subsystem documentation and future ADRs if needed.

---

## Validation

The decision is considered valid only if the following are proven in testing.

### Bench validation targets
- all 4 ADS1115 devices are detected reliably
- all 16 channels are readable and stable
- temperature conversion is repeatable and realistic enough for bench interpretation
- sensor disconnect / short-like fault behavior can be detected
- no unacceptable channel instability appears during basic bench operation

### Pack-level validation targets
- sensors can be mounted repeatably to the intended pack positions
- readings respond to real thermal changes
- wiring remains manageable in actual pack assembly
- the system does not become mechanically fragile or overly complex

### Success criteria
- 16 working channels
- stable measurement behavior
- clear benefit over BMS-only monitoring
- practical physical installation inside battery system

### Failure / revisit triggers
Revisit this ADR if:
- sensor mounting proves impractical
- analog noise is too high
- ADS1115 behavior is insufficient for the intended monitoring role
- wiring complexity becomes unreasonable
- measured value quality is not good enough for meaningful warning logic
- the generic module approach proves unsuitable beyond bench or prototype use

---

## References

### Related project facts
- Custom camper LiFePO4 battery project
- Reclaimed LiFePO4 32650 cells
- 4P4S pack architecture
- JK BMS used for electrical battery telemetry
- Dedicated BTM subsystem added for independent thermal monitoring

### Selected hardware baseline
- ESP32-WROOM-32 class development board
- USB-C
- CH340C USB-UART bridge
- ADS1115 x4
- B3950-10K-L25MM NTC x16
- 10 kOhm 1% 0.4 W resistor x16

### Related supporting documents
- BTM bench test plan
- BTM bench setup requirements
- BTM wiring notes
- BTM bench firmware skeleton

---

## Boundary Note

This ADR covers the **need for a dedicated BTM subsystem** and the **currently selected bench hardware baseline** for the first implementation phase.

It does **not** yet define:
- the communication hardware used to connect BTM to the rest of the system
- the transport layer
- the data protocol
- message structure
- heartbeat rules
- ownership boundaries between BTM preprocessing and VCU logic
- final production hardware architecture

Those topics are intentionally deferred and will be resolved later under the **MCS (Messaging & Communication System)** architecture and related follow-up ADRs.

Cross-links between BTM, MCS, and wider system documentation should be added as the documentation set matures.

## Revision History

| Date | Author | Description |
|------|--------|-------------|
| 2026-04-02 | SawGreen / ChatGPT | Initial draft |
| 2026-04-03 | ChatGPT | Revised to reflect current bench hardware baseline and bench-phase implementation assumptions |