# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.2] - 2025-12-22

### Changed

- Extended message delays to prevent dropped communications

## [2.0.1]

### Added

- Github Action to build payloads
- CLI options to toggle commands to allow for setting operation type, trip control codes, and starting toggle 
  sequences with a close operation

### Changed

- Changed Toggle Activation Model behavior to use trip control codes

## [2.0.0]

### Added

- DNP3 serial functionality
- Entirely new DNP3 Trainer (dnp3-outstation) to replace the previous testing server
- TCP and Serial versions of every ability
- The following abilities:
  - DNP3 (TCP|Serial) Operate
  - DNP3 (TCP|Serial) Toggle (Two Output Model)
  - DNP3 (TCP|Serial) Toggle (Activation Model)
  - DNP3 (TCP|Serial) Read All
- This CHANGELOG 

### Changed

- Refactor CLI code to use callback feature of CLI11 library
- Redesign actions code related to operating and toggling points to better reflect different DNP3 operating models and simplify the CLI
- Update abilities to conform with new CLI
- Update fact names and example fact source to impose a clearer naming convention
- Update documentation to reflect these changes
- Change logging format to allow for easier parsing
- Change build system to enable builds on Windows, Linux, and Darwin (macOS)

### Removed

- dnp3-poll source code and binary
- dnp3-server source code and binary (see the new DNP3 Trainer: dnp3-outstation)
- The following abilities have been removed due to unclear naming and implementation:
  - DNP3 Ranged Modulate Breaker SBO (replace with DNP3 Toggle)
  - DNP3 Modulate Breaker SBO (replace with DNP3 Toggle)
  - DNP3 Toggle OFF Breakers SBO (replace with DNP3 Operate)
  - DNP3 Toggle ON Breakers SBO (replace with DNP3 Operate)
  - DNP3 Modulate Breaker DO (replace with DNP3 Toggle)
  - DNP3 Toggle OFF Breakers DO (replace with DNP3 Operate)
  - DNP3 Toggle ON Breakers DO (replace with DNP3 Operate)


## [1.1.0] - 2024-09-18

### Added

- Caldera v5 (Magma) GUI.

## [1.0.1] - 2023-11-13

### Changed

- Fix multiline yaml syntax bug that caused some abilities to fail
- Replace all instances of CALDERA with Caldera to match new style guidelines

## [1.0.0] - 2023-07-17

### Added

- Initial stable version of the DNP3 plugin, part of [MITRE Caldera™ for OT](https://github.com/mitre/caldera-ot). Provides 13 unique Caldera adversary emulation abilities specific to the DNP3 protocol.

[unreleased]: https://github.com/mitre/dnp3/compare/v2.0.2...HEAD
[2.0.2]: https://github.com/mitre/dnp3/compare/v2.0.1...v2.0.2
[2.0.1]: https://github.com/mitre/dnp3/compare/v2.0.0...v2.0.1
[2.0.0]: https://github.com/mitre/dnp3/compare/v1.1.0...v2.0.0
[1.1.0]: https://github.com/mitre/dnp3/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/mitre/dnp3/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/mitre/dnp3/releases/tag/v1.0.0
