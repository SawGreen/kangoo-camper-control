# Project Roadmap

## 1. Overview

This roadmap defines the phased development of the **Kangoo Camper Control System (KCCS)**. Each phase builds upon the previous one, emphasizing modular development, validation, and safe integration.

---

## 2. Development Phases

### Phase 0 — Project Definition
**Objective:** Establish project foundations and documentation.

**Deliverables:**
- Repository structure
- System overview documentation
- Subsystem definitions (HMI, VCU, BTM, ETM, MCS)
- Decision log (ADR framework)
- Validation strategy
- Initial scope definition (v0.1)

**Status:** Completed / In Progress

---

### Phase 1 — Interface & Architecture Decisions
**Objective:** Define system interfaces and communication strategy.

**Deliverables:**
- Communication architecture (CAN / RS485 / UART)
- Message model and signal naming
- Node identification and addressing
- HMI platform decision (ESP32 vs Raspberry Pi)
- Sensor technology selection (NTC/PT1000)
- Power supply topology

---

### Phase 2 — Proof of Concept (PoC)
**Objective:** Validate key technologies through isolated experiments.

**PoC Targets:**
- JK BMS telemetry acquisition
- Battery cell temperature measurement (ADS1115 + sensors)
- Engine temperature sensing (coolant and oil)
- Rotary encoder navigation and display prototype
- Communication bus validation (CAN/RS485/UART)

**Deliverables:**
- Test reports and validation logs
- Hardware evaluation results
- Updated BOM and decisions

---

### Phase 3 — Integration Skeleton
**Objective:** Establish a functional system backbone.

**Deliverables:**
- Messaging framework (MCS)
- Signal store and message definitions
- Heartbeat and watchdog mechanisms
- Warning and event management framework
- Basic VCU firmware skeleton
- Shared libraries and protocols

---

### Phase 4 — Functional Integration
**Objective:** Integrate subsystems into a cohesive system.

**Deliverables:**
- JK BMS integration with VCU
- BTM integration and validation
- ETM integration and validation
- Fan assist control implementation
- HMI integration with VCU
- Initial system-level testing

---

### Phase 5 — Vehicle Validation
**Objective:** Validate the system in real vehicle conditions.

**Test Scenarios:**
- Bench testing and simulation
- Cold start testing
- Thermal load testing
- Long-distance driving tests
- Hill climb and high-altitude validation
- Failure mode and safety testing

**Deliverables:**
- Validation reports
- Performance logs
- System tuning and calibration

---

### Phase 6 — Release v0.1
**Objective:** Deliver the first functional version of the system.

**Features:**
- JK BMS telemetry integration
- Battery cell temperature monitoring
- Coolant and oil temperature monitoring
- Central HMI display
- Manual and automatic fan assist control
- Warning and event logging
- Messaging and communication framework

---

### Phase 7 — Future Development
**Potential Enhancements:**
- OBD2 data integration
- Advanced data logging and analytics
- Predictive thermal management
- Wireless connectivity (Wi-Fi/Bluetooth)
- Mobile application interface
- Cloud data synchronization
- Additional vehicle sensors

---

## 3. Milestones

| Milestone | Description |
|-----------|-------------|
| M0 | Repository and documentation initialized |
| M1 | Communication architecture selected |
| M2 | Key PoCs validated |
| M3 | Integration skeleton completed |
| M4 | Functional system integration |
| M5 | Vehicle validation completed |
| M6 | Release v0.1 |
| M7 | Expansion and future features |

---

## 4. Success Criteria

- All subsystems operate independently and reliably
- Safety-critical functions remain operational without HMI
- Communication between nodes is robust and fault-tolerant
- System withstands automotive environmental conditions
- Documentation and decision logs remain complete and up-to-date