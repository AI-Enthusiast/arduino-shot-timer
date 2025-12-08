# Constants Quick Reference Guide

## Quick Lookup Table

### Hardware Configuration
| Constant | Value | Purpose |
|----------|-------|---------|
| `kMicPin` | A0 | Microphone input pin |
| `kButtonDur` | 80 | Button duration |
| `kBeepDur` | 400 | Beep duration |
| `kBeepNote` | NOTE_C4 | Beep note frequency |

### LCD Display
| Constant | Value | Purpose |
|----------|-------|---------|
| `LCD_COLS` | 16 | LCD width in characters |
| `LCD_ROWS` | 2 | LCD height in rows |
| `LCD_DISPLAY_WIDTH_2` | 2 | 2-digit display width |
| `LCD_DISPLAY_WIDTH_3` | 3 | 3-digit display width (shot counter) |
| `LCD_TIME_WIDTH` | 9 | Time display width (mm:ss.ms) |

### Timing and Delays
| Constant | Value | Purpose |
|----------|-------|---------|
| `MILLIS_PER_SECOND` | 1000 | Milliseconds in one second |
| `RANDOM_DELAY_1TO4_MIN` | 1000 | Min delay for random 1-4s (1s) |
| `RANDOM_DELAY_1TO4_MAX` | 4001 | Max delay for random 1-4s (4s) |
| `RANDOM_DELAY_2TO6_MIN` | 2000 | Min delay for random 2-6s (2s) |
| `RANDOM_DELAY_2TO6_MAX` | 6001 | Max delay for random 2-6s (6s) |
| `DELAY_SETTING_MAX` | 12 | Maximum delay setting value |
| `DELAY_SETTING_RANDOM_1TO4` | 11 | Setting for random 1-4s |
| `DELAY_SETTING_RANDOM_2TO6` | 12 | Setting for random 2-6s |

### Par Time Increments
| Constant | Value | Purpose |
|----------|-------|---------|
| `PAR_TIME_INCREMENT_1MS` | 1 | 1 millisecond increment |
| `PAR_TIME_INCREMENT_10MS` | 10 | 10 millisecond increment |
| `PAR_TIME_INCREMENT_100MS` | 100 | 100 millisecond increment |
| `PAR_TIME_INCREMENT_1S` | 1000 | 1 second increment |
| `PAR_TIME_INCREMENT_10S` | 10000 | 10 second increment |
| `PAR_TIME_INCREMENT_1MIN` | 60000 | 1 minute increment |
| `PAR_TIME_INCREMENT_10MIN` | 600000 | 10 minute increment |

### Par Time Maximum Values
| Constant | Value | Purpose |
|----------|-------|---------|
| `PAR_TIME_MAX` | 5999999 | Absolute maximum (99:59.999) |
| `PAR_TIME_MAX_1MS` | 5999999 | Max when editing milliseconds |
| `PAR_TIME_MAX_10MS` | 5999990 | Max when editing 10ms |
| `PAR_TIME_MAX_100MS` | 5999900 | Max when editing 100ms |
| `PAR_TIME_MAX_1S` | 5999000 | Max when editing seconds |
| `PAR_TIME_MAX_10S` | 5990000 | Max when editing 10 seconds |
| `PAR_TIME_MAX_1MIN` | 5940000 | Max when editing minutes |
| `PAR_TIME_MAX_10MIN` | 5400000 | Max when editing 10 minutes |

### Par Time Minimum Values
| Constant | Value | Purpose |
|----------|-------|---------|
| `PAR_TIME_MIN_1MS` | 1 | Min when editing milliseconds |
| `PAR_TIME_MIN_10MS` | 10 | Min when editing 10ms |
| `PAR_TIME_MIN_100MS` | 100 | Min when editing 100ms |
| `PAR_TIME_MIN_1S` | 1000 | Min when editing seconds |
| `PAR_TIME_MIN_10S` | 10000 | Min when editing 10 seconds |
| `PAR_TIME_MIN_1MIN` | 60000 | Min when editing minutes |
| `PAR_TIME_MIN_10MIN` | 600000 | Min when editing 10 minutes |

### Par Cursor Positions
| Constant | Value | Purpose |
|----------|-------|---------|
| `PAR_CURSOR_1MS` | 1 | Cursor position: milliseconds |
| `PAR_CURSOR_10MS` | 2 | Cursor position: 10 milliseconds |
| `PAR_CURSOR_100MS` | 3 | Cursor position: 100 milliseconds |
| `PAR_CURSOR_1S` | 4 | Cursor position: seconds |
| `PAR_CURSOR_10S` | 5 | Cursor position: 10 seconds |
| `PAR_CURSOR_1MIN` | 6 | Cursor position: minutes |
| `PAR_CURSOR_10MIN` | 7 | Cursor position: 10 minutes |
| `PAR_CURSOR_MIN` | 1 | Minimum cursor position |
| `PAR_CURSOR_MAX` | 7 | Maximum cursor position |
| `PAR_CURSOR_DEFAULT` | 4 | Default cursor position (seconds) |

### Settings Ranges
| Constant | Value | Purpose |
|----------|-------|---------|
| `BEEP_VOL_MIN` | 0 | Minimum beep volume |
| `BEEP_VOL_MAX` | 10 | Maximum beep volume |
| `SENSITIVITY_MIN` | 0 | Minimum sensitivity |
| `SENSITIVITY_MAX` | 20 | Maximum sensitivity |
| `SAMPLE_WINDOW_MIN` | 10 | Minimum echo protection (ms) |
| `SAMPLE_WINDOW_MAX` | 100 | Maximum echo protection (ms) |
| `SAMPLE_WINDOW_STEP` | 10 | Echo protection step size (ms) |

### Threshold Calibration
| Constant | Value | Purpose |
|----------|-------|---------|
| `THRESHOLD_BASE` | 650 | Base threshold value |
| `THRESHOLD_SENSITIVITY_MULTIPLIER` | 25 | Sensitivity adjustment multiplier |
| `THRESHOLD_DEFAULT` | 625 | Default threshold (650 - 25*1) |

### Default Settings
| Constant | Value | Purpose |
|----------|-------|---------|
| `DEFAULT_DELAY_TIME` | 11 | Default delay setting (random 1-4s) |
| `DEFAULT_BEEP_VOL` | 10 | Default beep volume (max) |
| `DEFAULT_SENSITIVITY` | 1 | Default sensitivity level |
| `DEFAULT_SAMPLE_WINDOW` | 50 | Default echo protection (ms) |

---

## How to Use This Guide

### Finding a Constant
1. Identify what you're looking for (e.g., "LCD width")
2. Check the appropriate category
3. Find the constant name
4. Use it in your code

### Modifying a Value
1. Find the constant in the table above
2. Locate it in ShotTimer.ino (top of file)
3. Change the value
4. Recompile - all uses update automatically

### Adding a New Constant
1. Choose descriptive name following naming conventions
2. Add to appropriate category section
3. Update this reference guide
4. Use throughout code

---

## Naming Conventions

### Pattern 1: Hardware and Config (PROGMEM)
```cpp
const type_p PROGMEM kName = value;
```
Example: `const uint8_p PROGMEM kMicPin = A0;`

### Pattern 2: Display Constants
```cpp
const uint8_t LCD_PROPERTY = value;
```
Example: `const uint8_t LCD_COLS = 16;`

### Pattern 3: Time Constants
```cpp
const type PAR_TIME_ASPECT_UNIT = value;
```
Example: `const unsigned long PAR_TIME_INCREMENT_1S = 1000;`

### Pattern 4: Cursor Constants
```cpp
const uint8_t PAR_CURSOR_UNIT = value;
```
Example: `const uint8_t PAR_CURSOR_1MS = 1;`

### Pattern 5: Settings Constants
```cpp
const uint8_t SETTING_NAME_MINMAX = value;
```
Example: `const uint8_t BEEP_VOL_MAX = 10;`

### Pattern 6: Default Values
```cpp
const type DEFAULT_NAME = value;
```
Example: `const uint8_t DEFAULT_BEEP_VOL = 10;`

---

## Common Use Cases

### Changing Delay Ranges
```cpp
// Want random delay 3-8 seconds instead of 2-6?
const uint16_t RANDOM_DELAY_2TO6_MIN = 3000;  // Change from 2000
const uint16_t RANDOM_DELAY_2TO6_MAX = 8001;  // Change from 6001
```

### Adjusting Sensitivity Calibration
```cpp
// Need different sensitivity curve for new microphone?
const int THRESHOLD_BASE = 700;                      // Adjust base
const int THRESHOLD_SENSITIVITY_MULTIPLIER = 30;     // Adjust multiplier
```

### Supporting Different LCD
```cpp
// Using a 20x4 LCD instead of 16x2?
const uint8_t LCD_COLS = 20;  // Change from 16
const uint8_t LCD_ROWS = 4;   // Change from 2
```

### Extending Par Time Limit
```cpp
// Want to support up to 20 minutes instead of 99:59.999?
const unsigned long PAR_TIME_MAX = 1199999;  // 19:59.999
const unsigned long PAR_TIME_MAX_10MIN = 1200000;  // Update 10min max
```

### Changing Beep Volume Range
```cpp
// Want volume from 0-15 instead of 0-10?
const uint8_t BEEP_VOL_MAX = 15;  // Change from 10
```

---

## Related Constants Groups

### Par Time Editing
When editing par times, these constants work together:
- **Cursor Position**: `PAR_CURSOR_*` (which digit being edited)
- **Increment**: `PAR_TIME_INCREMENT_*` (how much to add/subtract)
- **Maximum**: `PAR_TIME_MAX_*` (upper limit for this position)
- **Minimum**: `PAR_TIME_MIN_*` (lower limit for this position)

Example: When cursor is at `PAR_CURSOR_1S`:
- Increment by `PAR_TIME_INCREMENT_1S` (1000ms)
- Max value is `PAR_TIME_MAX_1S` (5999000)
- Min value is `PAR_TIME_MIN_1S` (1000)

### Delay Settings
These constants define the delay behavior:
- **Setting Value**: 0-10 = fixed seconds, 11 = random 1-4s, 12 = random 2-6s
- **Comparison**: `DELAY_SETTING_RANDOM_1TO4`, `DELAY_SETTING_RANDOM_2TO6`
- **Random Ranges**: `RANDOM_DELAY_*_MIN/MAX`
- **Conversion**: `MILLIS_PER_SECOND` to convert setting to milliseconds

### Sensitivity System
These constants control microphone sensitivity:
- **Range**: `SENSITIVITY_MIN` to `SENSITIVITY_MAX`
- **Default**: `DEFAULT_SENSITIVITY`
- **Threshold Calculation**: `THRESHOLD_BASE - (THRESHOLD_SENSITIVITY_MULTIPLIER * sensitivity)`
- **Result**: Stored in `g_threshold`

---

## Tips and Tricks

### Finding Constant Usage
```bash
# Find all uses of a constant
grep -n "CONSTANT_NAME" ShotTimer.ino
```

### Validating Constants
Make sure related constants are consistent:
```cpp
// These should match
PAR_CURSOR_MIN == PAR_CURSOR_1MS  // Both should be 1
PAR_CURSOR_MAX == PAR_CURSOR_10MIN  // Both should be 7

// Threshold calculation
THRESHOLD_DEFAULT == THRESHOLD_BASE - (THRESHOLD_SENSITIVITY_MULTIPLIER * DEFAULT_SENSITIVITY)
// 625 == 650 - (25 * 1) ✓
```

### Memory Usage
- Constants marked `PROGMEM` are stored in Flash, not RAM
- Regular constants are compile-time, no runtime cost
- Total memory impact: ~100 bytes Flash, 0 bytes RAM

---

## Before/After Quick Reference

| Old Magic Number | New Constant | Where Used |
|------------------|--------------|------------|
| `1` | `DEFAULT_SENSITIVITY` | g_sensitivity initialization |
| `1` | `PAR_CURSOR_DEFAULT` (actually 4) | g_par_cursor initialization |
| `10` | `DEFAULT_BEEP_VOL` | g_beep_vol initialization |
| `11` | `DEFAULT_DELAY_TIME` | g_delay_time initialization |
| `11` | `DELAY_SETTING_RANDOM_1TO4` | StartDelay() comparison |
| `12` | `DELAY_SETTING_MAX` | IncreaseDelay(), DecreaseDelay() |
| `50` | `DEFAULT_SAMPLE_WINDOW` | g_sample_window initialization |
| `625` | `THRESHOLD_DEFAULT` | g_threshold initialization |
| `650` | `THRESHOLD_BASE` | SensToThreshold() calculation |
| `25` | `THRESHOLD_SENSITIVITY_MULTIPLIER` | SensToThreshold() calculation |
| `1000` | `MILLIS_PER_SECOND` | StartDelay() conversion |
| `2000` | `RANDOM_DELAY_2TO6_MIN` | StartDelay() random range |
| `6001` | `RANDOM_DELAY_2TO6_MAX` | StartDelay() random range |
| `1000` | `RANDOM_DELAY_1TO4_MIN` | StartDelay() random range |
| `4001` | `RANDOM_DELAY_1TO4_MAX` | StartDelay() random range |
| `16` | `LCD_COLS` | LCDSetup() |
| `2` | `LCD_ROWS` | LCDSetup() |
| `9` | `LCD_TIME_WIDTH` | All lcd_print_time() calls |
| `2` | `LCD_DISPLAY_WIDTH_2` | Par number displays |
| `3` | `LCD_DISPLAY_WIDTH_3` | Shot counter display |

---

**Last Updated:** December 7, 2025  
**Total Constants:** 61  
**Categories:** 8

