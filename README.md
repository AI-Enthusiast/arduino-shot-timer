# Arduino Shot Timer

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

A shot timer built with Arduino that listens for shots and provides detailed timing analysis for shooting practice.

**Original Author:** hestenet  
**Forked From:** [hestenet/arduino-shot-timer](https://github.com/hestenet/arduino-shot-timer)

## Features

- **Shot Recording:** Records up to 200 shots in a string, with support for strings lasting into double-digit hours
- **Review Mode:** Displays shot number, time, and split time with previous shot
  - Can also display average split and rate of fire (shots/min)
- **Configurable Par Times:**
  - Toggle par times on/off
  - Set up to 10 additive par beep times
- **Settings:**
  - Start delay options: Random 1-4s, Random 2-6s, or 0-10 seconds
  - Rate of fire calculation (configurable to include/exclude draw time)
  - Beep volume control
  - Sensitivity adjustment (includes dry fire options)
  - Echo rejection
- **Persistent Settings:** All settings are preserved via EEPROM when the device is powered off

## Hardware Requirements

| Component | Description |
|-----------|-------------|
| Arduino Uno R3 | Main microcontroller |
| [Adafruit RGB LCD Shield](https://www.adafruit.com/products/714) | Display interface |
| [Adafruit Electret Mic/Amp](https://www.adafruit.com/products/1063) | Sound detection |
| Piezo Buzzer | Audio feedback |

## Dependencies

### External Libraries (via Arduino Library Manager)

- `MenuSystem.h` - Menu navigation
- `Chrono.h` (LightChrono) - Timing functionality
- `PGMWrap.h` - PROGMEM string storage
- `EEWrap.h` - EEPROM settings management
- `Adafruit_RGBLCDShield.h` - LCD shield control
- `Wire.h` - I2C communication
- `avr/pgmspace.h` - Program memory utilities

### External Source

- [`toneAC`](https://bitbucket.org/teckel12/arduino-toneac/) - Enhanced tone generation

### Internal Libraries

- `DebugMacros.h` - Debug toggle functionality
- `Pitches.h` - Musical note definitions
- `LegibleTime.h` - Time formatting (converts `millis()` to `00:00:00.000` format)
- `LCDHelpers.h` - LCD printing utilities for PROGMEM data, time, and formatted numbers

## Known Issues

- Enabling `DEBUG` mode may cause out-of-memory (OOM) errors due to excessive debug statements, particularly when editing par times or listening for shots.

## Memory Usage

**Version 2.0.0** (without `DEBUG`):
- **PROGMEM:** 17,652 bytes (54%)
- **SRAM:** 1,467 bytes (71%)

> **Note:** The majority of SRAM usage comes from `uint32_t shotTimes[200]` (800 bytes) and `uint32_t parTimes[10]` (40 bytes).

## Release History

### [2.0.0](https://github.com/hestenet/arduino-shot-timer/releases/tag/v.2.0.0) - EEPROM Release

The 2.x branch stores settings in EEPROM and does not require an SD card interface.

**Major changes:**
- Compatibility with Arduino IDE v1.6.8
- Replaced legacy libraries with ones available through the Arduino Library Manager
- Internal code refactoring and library organization
- Reduced memory overhead

### Deprecated Releases

#### ~~[1.0.0](https://github.com/hestenet/arduino-shot-timer/releases/tag/v1.0.0)~~

Initial build from code.google.com migration. Contains incompatibilities with newer Arduino IDE versions due to the `menuBackend` library.

## License

This project is licensed under the [LGPL v3](https://www.gnu.org/licenses/lgpl-3.0) license.