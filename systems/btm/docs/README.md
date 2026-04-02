# BTM — Battery Temperature Monitoring

## 1. Purpose

The **BTM (Battery Temperature Monitoring)** subsystem is responsible for independent temperature measurement and validation of all individual cell positions in the custom LiFePO4 battery pack.

BTM exists as a dedicated subsystem because the battery is built from reclaimed LiFePO4 32650 cells in a **4P4S** configuration, and therefore requires better thermal visibility than a standard integrated commercial battery solution.

The purpose of BTM is to provide:

- cell-level thermal visibility
- early detection of abnormal local heating
- independent monitoring outside of the JK BMS
- normalized thermal telemetry for the wider system
- support for warning generation, diagnostics, and future control logic

---

## 2. Scope

### In scope
- individual temperature measurement of all 16 cell positions
- analog sensor acquisition
- local preprocessing of temperature data
- sensor plausibility checks
- filtered value generation
- local subsystem health monitoring
- export of normalized thermal data to the rest of the system

### Out of scope
- battery electrical protection logic
- SOC/SOH estimation
- charge/discharge control logic
- final system-wide warning arbitration
- HMI presentation logic
- communication protocol definition
- final transport/interface selection to the rest of the system

Electrical battery protection and core battery telemetry remain the responsibility of the **JK BMS** and the wider control architecture.

System-wide communication design belongs to **MCS (Messaging & Communication System)**.

---

## 3. Battery Context

The monitored battery pack is based on:

- **cell type:** LiFePO4 32650
- **cell source:** reclaimed from unused model battery packs
- **pack configuration:** 4P4S
- **total cell count:** 16

Because the pack is custom-built from reclaimed cells, the project requires more detailed thermal insight than would typically be needed for a factory-integrated pack.

---

## 4. Functional Responsibilities

BTM is expected to provide at minimum:

- 16 raw temperature channels
- converted temperature values
- filtered temperature values
- minimum detected temperature
- maximum detected temperature
- pack temperature delta
- sensor fault indication
- node health / alive indication

Depending on later MCS and VCU decisions, BTM may also provide:

- sensor disconnect detection
- short-circuit detection
- per-channel plausibility flags
- local warning suggestion flags

---

## 5. Current Hardware Baseline

The currently selected hardware baseline is:

- **1x ESP32**
- **4x ADS1115**
- **16x B3950-10K-L25MM NTC thermistors**
- **16x 10 kOhm 1% 0.4 W resistors**

This hardware selection is documented in:

- `docs/decisions/ADR-001-btm-temperature-acquisition.md`

---

## 6. Logical Block View

```text
[Cell 1 NTC] \
[Cell 2 NTC]  \
[Cell 3 NTC]   >--[Voltage divider network]--[ADS1115 x4]--[ESP32]--[BTM output]
...           /
[Cell 16 NTC]/

```

BTM is responsible for the measurement chain up to the point where normalized subsystem data is handed over to the wider system.

The definition of that handover interface is not part of BTM alone and will be resolved later within MCS.

---

## 9. Assumptions

The current BTM concept assumes:

- one temperature sensor per physical cell position
- thermal trends are more important than ultra-fast sampling
- measurement robustness is more important than aggressive optimization
- additional thermal visibility is justified because reclaimed cells are used
- BTM should remain testable as a standalone subsystem before full integration

---

## 10. Validation Strategy

BTM must be validated in stages.

**Stage 1 — Measurement chain validation**
- read all 16 channels successfully
- confirm stable measurements
- verify realistic temperature conversion
- test disconnected and faulty sensor behavior

**Stage 2 — Mechanical validation**
- confirm sensor mounting feasibility
- verify repeatability of installation
- inspect harness complexity and maintainability

**Stage 3 — Thermal response validation**
- confirm that measured values react to real thermal change
- evaluate lag and consistency
- compare channels under similar conditions

**Stage 4 — System integration validation**
- validate handover of telemetry to the wider system
- validate node health reporting
- validate behavior under communication faults

---

## 11. Documentation Boundaries

This document describes the **BTM subsystem itself**.

It does **not** define:

- the final communication bus
- final message protocol
- final inter-node transport layer
- full system-level warning arbitration

Those topics belong to:

- systems/mcs/...
- future MCS-related ADRs
- future VCU integration documents

Cross-links between subsystem documentation and architecture decisions will be added progressively as the repository matures.

---

## 12. Related Documents

**Current**
- docs/system-overview.md
- docs/roadmap.md
- docs/decisions/ADR-001-btm-temperature-acquisition.md

**Planned / Future**
- MCS communication architecture document
- VCU subsystem overview
- BTM hardware wiring notes
- BTM validation report
- BTM calibration notes

---