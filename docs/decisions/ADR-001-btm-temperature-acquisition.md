# ADR-001: Battery Temperature Monitoring Architecture for 4P4S LiFePO4 Pack

- **Status:** Accepted
- **Date:** 2026-04-02
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

The selected JK BMS provides electrical battery telemetry and protection functions, but the project requires **additional independent cell-level temperature monitoring** beyond the BMS.

The main reasons are:

- increased confidence when using reclaimed cells
- thermal visibility at individual-cell level
- ability to detect abnormal thermal behavior inside the pack
- ability to generate custom warnings and future control logic independent of BMS internal limits
- better support for diagnostics, testing, and long-term monitoring

This led to the decision to create a dedicated **BTM (Battery Temperature Monitoring)** subsystem.

---

## Decision

The battery temperature monitoring subsystem will use:

- **1x ESP32** as the local processing and communication node
- **4x ADS1115** I2C analog-to-digital converters
- **16x B3950-10K-L25MM NTC thermistors**
- **16x 10 kOhm 1% 0.4 W resistors**

The subsystem will measure **temperature of each individual cell position**, resulting in **16 independent temperature channels**.

The BTM subsystem will be treated as a dedicated system block responsible for:

- raw sensor acquisition
- value filtering
- sensor plausibility checks
- temperature warning generation
- communication of normalized values to the VCU

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

### Option B — Dedicated BTM with one sensor per cell using ESP32 + ADS1115 + NTC
**Description:**  
Build an independent temperature monitoring node capable of measuring all 16 cell positions.

**Advantages:**
- full cell-level thermal visibility
- independence from BMS thermal sensing
- better diagnostics during pack development and validation
- supports custom warnings and future logic
- scalable and modular subsystem design

**Disadvantages:**
- increased hardware count
- increased wiring complexity
- analog measurement chain requires calibration and noise handling
- requires dedicated firmware and integration work

---

### Option C — Use digital temperature sensors instead of NTC + ADC
**Description:**  
Use digital sensors such as 1-Wire or I2C temperature sensors for each cell position.

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

1. **Cell-level thermal visibility**
2. **Independent monitoring outside the BMS**
3. **Suitability for reclaimed-cell pack architecture**
4. **Diagnostic value during development**
5. **Future expandability**

The pack is not a standard drop-in commercial battery module. It is a custom-built pack from reused cells, which increases the importance of observing thermal behavior more closely than in a conventional integrated battery pack.

Using one temperature channel per cell position provides significantly better insight during:

- first assembly validation
- charge/discharge testing
- balancing observation
- fault detection
- long-term pack health monitoring

ESP32 was selected as the local controller because it is already aligned with the wider system architecture and is suitable for handling acquisition, preprocessing, and communication.

ADS1115 was selected because it provides a practical way to expand analog channel count in a modular way. Four devices provide the required 16 input channels.

NTC thermistors with matching resistor dividers were selected as a practical, available, and cost-effective approach for distributed temperature sensing inside the battery pack.

---

## Consequences

### Positive
- each cell position gets its own monitored temperature channel
- improved trust in pack operation
- better ability to detect abnormal local heating
- BTM becomes a reusable and testable standalone subsystem
- thermal data can be integrated into HMI, VCU warnings, and logging

### Negative
- more hardware to buy, wire, mount, and validate
- more firmware complexity
- analog measurements require calibration strategy
- I2C and analog routing must be designed carefully
- mechanical attachment of 16 sensors becomes an important design task

### Neutral
- BTM is now a first-class subsystem and must be documented, tested, and versioned like the others
- integration with MCS and VCU must be defined later

---

## Implementation Notes

### Functional scope of BTM
The BTM subsystem is expected to provide at minimum:

- 16 raw temperature values
- filtered temperature values
- min temperature
- max temperature
- temperature delta across pack
- sensor fault flags
- node health / heartbeat

### Hardware scope
Initial hardware baseline:

- ESP32
- 4x ADS1115
- 16x B3950-10K-L25MM NTC
- 16x 10k 1% resistor

### Open implementation topics
This ADR does **not** yet finalize:

- exact ESP32 variant
- PCB vs modular board layout
- exact thermistor mounting method on cells
- cable harness design
- shielding / analog grounding strategy
- calibration coefficients and temperature conversion model
- final communication interface from BTM to VCU

Those items will be handled in follow-up subsystem documentation and future ADRs if needed.

---

## Validation

The decision is considered valid only if the following are proven in testing:

### Bench validation targets
- all 16 channels readable and stable
- full scan rate acceptable for battery monitoring use
- no unacceptable drift between channels
- sensor disconnect / short fault can be detected
- temperature conversion is repeatable and realistic

### Pack-level validation targets
- sensors can be mounted repeatably to all cell positions
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
- ADS1115 update behavior is insufficient
- wiring complexity becomes unreasonable
- measured value quality is not good enough for meaningful warning logic

---

## References

### Related project facts
- Custom camper LiFePO4 battery project
- Reclaimed LiFePO4 32650 cells
- 4P4S pack architecture
- JK BMS used for electrical battery telemetry
- Dedicated BTM subsystem added for independent thermal monitoring

### Selected hardware
- ESP32
- ADS1115 x4
- B3950-10K-L25MM NTC x16
- 10 kOhm 1% 0.4 W resistor x16

---


```markdown
## Boundary Note

This ADR covers the **need for a dedicated BTM subsystem** and the **selected local measurement hardware baseline**.

It does **not** yet define:
- the communication hardware used to connect BTM to the rest of the system
- the transport layer
- the data protocol
- message structure
- heartbeat rules
- ownership boundaries between BTM preprocessing and VCU logic

Those topics are intentionally deferred and will be resolved later under the **MCS (Messaging & Communication System)** architecture and related follow-up ADRs.

Cross-links between BTM, MCS, and wider system documentation will be added as the documentation set matures.

## Revision History

| Date | Author | Description |
|------|--------|-------------|
| 2026-04-02 | SawGreen / ChatGPT | Initial draft |