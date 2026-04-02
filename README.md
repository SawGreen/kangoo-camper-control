# Kangoo Camper Control System

Modular monitoring and control system for a Renault Kangoo camper build.

## Project intent

This project is being developed as a structured, modular system rather than a single embedded sketch.

The goal is to build a reliable platform for:

- LiFePO4 battery telemetry
- Cell-level temperature monitoring
- Engine thermal monitoring
- Central non-touch HMI with rotary navigation
- Safe assist control functions such as early fan activation
- Expandable communication architecture between subsystems

## Main system blocks

### HMI — Human Machine Interface
User-facing display and control layer.

Scope:
- Central display
- Rotary encoder navigation
- Status pages
- Warning display
- Settings and user commands

### VCU — Vehicle Control Unit
Central logic and state aggregation layer.

Scope:
- Aggregation of subsystem data
- Warning handling
- Control logic
- System state management
- Safe command routing

### BTM — Battery Temperature Monitoring
Battery cell temperature acquisition and validation.

Scope:
- Cell temperature sensing
- Sensor validation
- Filtering and normalization
- Battery thermal warnings

### ETM — Engine Temperature Monitoring
Engine thermal sensing subsystem.

Scope:
- Coolant temperature monitoring
- Oil temperature monitoring
- Additional thermal inputs if needed
- Thermal warning support

### MCS — Messaging & Communication System
System-wide communication architecture.

Scope:
- Message model
- Communication topology
- Node identification
- Heartbeat and timeout rules
- Protocol definition between subsystems

## Repository structure

```text
docs/                 Global project documentation
systems/              Main system blocks
  hmi/                Human Machine Interface
  vcu/                Vehicle Control Unit
  btm/                Battery Temperature Monitoring
  etm/                Engine Temperature Monitoring
  mcs/                Messaging & Communication System
shared/               Shared assets, libraries, tools, simulation
logs/                 Validation notes, field logs, experiment outputs

```

## Project principles

- Safety-critical logic must not depend on HMI
- Every subsystem should be testable in isolation
- No hardware purchase without a defined test purpose
- Decisions must be documented with reasons
- Prefer maintainable architecture over clever shortcuts
- Prototype small, then integrate


## Initial focus

The first phase of the repository is not implementation-heavy.
The immediate goal is to define:

- system scope
- subsystem boundaries
- communication strategy
- test strategy
- decision log
- proof-of-concept path

Only after that should the project move into deeper hardware selection and firmware implementation.