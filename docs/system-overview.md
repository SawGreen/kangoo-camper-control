# System Overview

## 1. Introduction

The **Kangoo Camper Control System (KCCS)** is a modular monitoring and control platform designed for a Renault Kangoo camper conversion. The system integrates battery management telemetry, thermal monitoring, and vehicle assistance functions into a unified, extensible architecture.

The project emphasizes reliability, modularity, and maintainability, ensuring that safety-critical functions remain independent from the user interface while allowing flexible expansion and experimentation.

---

## 2. Objectives

### Primary Objectives
- Monitor LiFePO4 battery system telemetry via JK BMS
- Measure and validate cell-level temperatures
- Monitor engine thermal conditions (coolant and oil temperature)
- Provide a central non-touch Human Machine Interface (HMI)
- Enable safe assistive control functions (e.g., early radiator fan activation)
- Establish a robust messaging and communication architecture

### Secondary Objectives
- Support optional OBD2 data integration
- Provide data logging and diagnostics
- Enable modular hardware and firmware development
- Facilitate future system expansion

---

## 3. System Architecture

The system is composed of independent subsystems communicating through a unified messaging framework (MCS). Safety-critical logic is handled by the Vehicle Control Unit (VCU), while the HMI provides visualization and user interaction.


### High-Level Architecture

        +---------------------------+
        |            HMI            |
        |  Human Machine Interface  |
        +-------------+-------------+
                      |
                      |
        +-------------+-------------+
        |  MCS - Messaging Backbone |
========+=============+=============+=====================
        |             |             |             |
        |             |             |             |
   +----+----+   +----+----+   +----+----+   +---------+
   |   VCU   |   |   BTM   |   |   ETM   |   | JK BMS  |
   | Vehicle |   | Battery |   | Engine  |   | Battery |
   | Control |   | Temp    |   | Temp    |   | Mgmt    |
   | Unit    |   | Mon.    |   | Mon.    |   | System  |
   +---------+   +---------+   +---------+   +---------+


---

## 4. Subsystem Descriptions

### 4.1 HMI — Human Machine Interface
**Role:** User-facing display and control interface.

**Responsibilities:**
- Central system visualization
- Rotary encoder navigation
- Warning and alarm display
- Configuration and user commands
- System diagnostics and status pages

---

### 4.2 VCU — Vehicle Control Unit
**Role:** Central logic and state aggregation unit.

**Responsibilities:**
- Aggregation of subsystem telemetry
- Warning and event management
- Control logic and decision-making
- Fan assist control
- System state management
- Safe command routing
- Heartbeat supervision

---

### 4.3 BTM — Battery Temperature Monitoring
**Role:** Cell-level battery temperature acquisition.

**Responsibilities:**
- Measurement of individual cell temperatures
- Sensor validation and fault detection
- Data filtering and normalization
- Thermal warning generation
- Communication with VCU

---

### 4.4 ETM — Engine Temperature Monitoring
**Role:** Engine thermal monitoring subsystem.

**Responsibilities:**
- Coolant temperature monitoring
- Oil temperature monitoring
- Additional thermal inputs as required
- Thermal warning generation
- Support for fan assist logic

---

### 4.5 MCS — Messaging & Communication System
**Role:** System-wide communication and integration framework.

MCS represents the messaging architecture, transport rules, addressing, heartbeat, and protocol conventions interconnecting all major system nodes.

**Responsibilities:**
- Definition of message model and signal naming
- Communication topology and transport layers
- Node identification and addressing
- Heartbeat and timeout supervision
- Protocol definition between subsystems
- Versioning and compatibility management

**Note:** MCS is not a physical node but a system-level architectural layer.

---

## 5. External Interfaces

| Interface | Description | Status |
|----------|-------------|--------|
| JK BMS | Battery telemetry and protection data | Planned |
| OBD2 | Optional vehicle ECU data | Optional |
| Sensors (NTC/PT1000) | Engine and battery temperature sensing | Planned |
| Fan Control Output | Early radiator fan activation assist | Planned |
| Display Interface | HMI display communication | Planned |

---

## 6. Design Principles

- **Safety First:** Safety-critical logic must not depend on HMI.
- **Modularity:** Each subsystem must be testable in isolation.
- **Scalability:** Architecture supports future expansion.
- **Maintainability:** Prefer simple, well-documented solutions.
- **Reliability:** Automotive-aware design and fail-safe behavior.
- **Traceability:** All decisions and changes must be documented.
- **Prototype Small, Integrate Gradually:** Proof-of-concept before integration.

---

## 7. Initial Release Scope (v0.1)

The first release focuses on core monitoring and assistive functionality:

- JK BMS telemetry integration
- Cell temperature monitoring (BTM)
- Coolant and oil temperature monitoring (ETM)
- Central HMI display with rotary navigation
- Manual fan activation
- Automatic early fan assist logic
- Warning and event logging
- Messaging and communication framework (MCS)

---

## 8. Out of Scope for v0.1

- Full dependence on OBD2 data
- Cloud connectivity or mobile applications
- Predictive or AI-based control strategies
- Touchscreen user interface
- Advanced remote diagnostics

---

## 9. References

- Repository: https://github.com/SawGreen/kangoo-camper-control
- JK BMS Documentation
- Renault Kangoo 2002 Service Documentation