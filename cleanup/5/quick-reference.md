# LCD Helper Functions - Quick Reference Guide

## Overview

Consolidated LCD helper functions for the Arduino Shot Timer. These functions eliminate repetitive LCD operation patterns and provide a clean, consistent API.

## Function Categories

### 1. Display Header Functions

#### `lcd_display_header()`
Clear screen and display two-line header with Flash strings.

```cpp
lcd_display_header(&g_lcd, F("Line 1"), F("Line 2"));
```

**Use case:** Configuration screens, menu headers

#### `lcd_display_header_p()`
Clear screen and display two-line header (line 2 from PROGMEM).

```cpp
lcd_display_header_p(&g_lcd, F("Par Times"), kEnabled);
```

**Use case:** When line 2 is a PROGMEM constant

---

### 2. Positioned Display Functions

These functions set cursor position and display content in one call.

#### `lcd_display_at()`
Display a numeric value at specific position.

```cpp
lcd_display_at(&g_lcd, col, row, value, digits);
// Example:
lcd_display_at(&g_lcd, 9, 0, g_current_par + 1, 2);
```

**Parameters:**
- `col`: Column position (0-15)
- `row`: Row position (0-1)
- `value`: Numeric value to display
- `digits`: Number of digits to display

#### `lcd_display_time_at()`
Display a time value at specific position.

```cpp
lcd_display_time_at(&g_lcd, col, row, time_ms, digits);
// Example:
lcd_display_time_at(&g_lcd, 5, 1, g_par_times[0], 9);
```

**Parameters:**
- `col`: Column position (0-15)
- `row`: Row position (0-1)
- `time_ms`: Time in milliseconds
- `digits`: Width of time display (typically 9 for mm:ss.ms)

#### `lcd_display_p_at()`
Display a PROGMEM string at specific position.

```cpp
lcd_display_p_at(&g_lcd, col, row, kProgmemString);
// Example:
lcd_display_p_at(&g_lcd, 0, 0, kShotNum);
```

**Use case:** Displaying constants stored in PROGMEM

#### `lcd_display_f_at()`
Display a Flash string at specific position.

```cpp
lcd_display_f_at(&g_lcd, col, row, F("Text"));
// Example:
lcd_display_f_at(&g_lcd, 0, 0, F("Edit"));
```

**Use case:** Displaying inline Flash strings

#### `lcd_display_int_at()`
Display an integer at specific position.

```cpp
lcd_display_int_at(&g_lcd, col, row, int_value);
// Example:
lcd_display_int_at(&g_lcd, 0, 1, g_beep_vol);
```

**Use case:** Displaying simple integer values

---

### 3. Status Display Functions

#### `lcd_display_enabled_status()`
Display enabled/disabled status on line 2.

```cpp
lcd_display_enabled_status(&g_lcd, bool_value, kEnabled, kDisabled);
// Example:
lcd_display_enabled_status(&g_lcd, g_par_enabled, kEnabled, kDisabled);
```

**Parameters:**
- `enabled`: Boolean condition
- `enabled_p`: PROGMEM string to show when true
- `disabled_p`: PROGMEM string to show when false

**Use case:** Toggle settings screens (Par Times, Include Draw, etc.)

---

## Common Usage Patterns

### Pattern 1: Simple Two-Line Display
**Before:**
```cpp
g_lcd.clear();
g_lcd.setCursor(0, 0);
g_lcd.print(F("Sensitivity"));
g_lcd.setCursor(0, 1);
g_lcd.print(g_sensitivity);
```

**After:**
```cpp
lcd_display_header(&g_lcd, F("Sensitivity"), F(""));
lcd_display_int_at(&g_lcd, 0, 1, g_sensitivity);
```

---

### Pattern 2: Enable/Disable Display
**Before:**
```cpp
g_lcd.clear();
g_lcd.setCursor(0, 0);
g_lcd.print(F("Par Times"));
g_lcd.setCursor(0, 1);
if (g_par_enabled == false) {
  lcd_print_p(&g_lcd, kDisabled);
} else {
  lcd_print_p(&g_lcd, kEnabled);
}
```

**After:**
```cpp
lcd_display_header(&g_lcd, F("Par Times"), F(""));
lcd_display_enabled_status(&g_lcd, g_par_enabled, kEnabled, kDisabled);
```

---

### Pattern 3: Positioned Updates
**Before:**
```cpp
g_lcd.setCursor(13, 0);
lcd_print(&g_lcd, g_current_shot + 1, 3);
g_lcd.setCursor(7, 1);
lcd_print_time(&g_lcd, g_shot_times[g_current_shot], 9);
```

**After:**
```cpp
lcd_display_at(&g_lcd, 13, 0, g_current_shot + 1, 3);
lcd_display_time_at(&g_lcd, 7, 1, g_shot_times[g_current_shot], 9);
```

---

### Pattern 4: Complex Multi-Element Display
**Before:**
```cpp
g_lcd.clear();
g_lcd.setCursor(0, 0);
g_lcd.print(F("<<"));
g_lcd.setCursor(5, 0);
g_lcd.print(F("Par"));
g_lcd.setCursor(9, 0);
lcd_print(&g_lcd, g_current_par + 1, 2);
g_lcd.setCursor(4, 1);
if (g_current_par > 0) {
  lcd_print_p(&g_lcd, kPlus);
} else {
  lcd_print_p(&g_lcd, kSpace);
}
lcd_print_time(&g_lcd, g_par_times[g_current_par], 9);
```

**After:**
```cpp
g_lcd.clear();
lcd_display_f_at(&g_lcd, 0, 0, F("<<"));
lcd_display_f_at(&g_lcd, 5, 0, F("Par"));
lcd_display_at(&g_lcd, 9, 0, g_current_par + 1, 2);
lcd_display_p_at(&g_lcd, 4, 1, (g_current_par > 0) ? kPlus : kSpace);
lcd_display_time_at(&g_lcd, 5, 1, g_par_times[g_current_par], 9);
```

---

## Function Selection Guide

| What to Display | Function to Use |
|----------------|-----------------|
| Two-line header | `lcd_display_header()` |
| Header with PROGMEM line 2 | `lcd_display_header_p()` |
| Numeric value with padding | `lcd_display_at()` |
| Time value (mm:ss.ms) | `lcd_display_time_at()` |
| PROGMEM constant | `lcd_display_p_at()` |
| Flash string F("...") | `lcd_display_f_at()` |
| Simple integer | `lcd_display_int_at()` |
| Enabled/Disabled status | `lcd_display_enabled_status()` |

---

## LCD Screen Layout Reference

```
┌────────────────┐
│0123456789012345│ <- Row 0 (16 columns)
│0123456789012345│ <- Row 1 (16 columns)
└────────────────┘
```

**Common Positions:**
- Top-left: `(0, 0)`
- Top-right: `(15, 0)` or calculated
- Bottom-left: `(0, 1)`
- Bottom-right: `(15, 1)` or calculated

---

## Best Practices

1. **Use positioned functions instead of separate setCursor + print:**
   ```cpp
   // Good
   lcd_display_at(&g_lcd, 5, 1, value, 2);
   
   // Avoid
   g_lcd.setCursor(5, 1);
   lcd_print(&g_lcd, value, 2);
   ```

2. **Use header functions for full screen updates:**
   ```cpp
   // Good
   lcd_display_header(&g_lcd, F("Title"), F("Subtitle"));
   
   // Avoid
   g_lcd.clear();
   g_lcd.setCursor(0, 0);
   g_lcd.print(F("Title"));
   g_lcd.setCursor(0, 1);
   g_lcd.print(F("Subtitle"));
   ```

3. **Use status function for enable/disable toggles:**
   ```cpp
   // Good
   lcd_display_enabled_status(&g_lcd, flag, kEnabled, kDisabled);
   
   // Avoid
   g_lcd.setCursor(0, 1);
   if (flag) { lcd_print_p(&g_lcd, kEnabled); }
   else { lcd_print_p(&g_lcd, kDisabled); }
   ```

4. **Combine positioned functions for complex layouts:**
   Multiple `lcd_display_*_at()` calls are clearer than mixing setCursor/print

---

## Migration Checklist

When updating code to use new helpers:

- [ ] Replace `clear() + setCursor() + print()` patterns with `lcd_display_header()`
- [ ] Replace `setCursor() + lcd_print()` with `lcd_display_at()`
- [ ] Replace `setCursor() + lcd_print_time()` with `lcd_display_time_at()`
- [ ] Replace enable/disable conditionals with `lcd_display_enabled_status()`
- [ ] Verify column/row positions are correct
- [ ] Test on hardware if possible
- [ ] Check for any remaining manual cursor positioning

---

## Examples by Screen Type

### Configuration Screen (Single Value)
```cpp
void EnterBeepState() {
  lcd_display_header(&g_lcd, F("Beep Volume"), F(""));
  lcd_display_int_at(&g_lcd, 0, 1, g_beep_vol);
}
```

### Toggle Screen (Enabled/Disabled)
```cpp
void EnterParStateState() {
  lcd_display_header(&g_lcd, F("Par Times"), F(""));
  lcd_display_enabled_status(&g_lcd, g_par_enabled, kEnabled, kDisabled);
}
```

### Timer Screen (Multiple Elements)
```cpp
void StartTimer() {
  lcd_display_f_at(&g_lcd, 0, 0, F(" GO!!  Shot#    "));
  lcd_display_f_at(&g_lcd, 0, 1, F("Last:"));
  BEEP();
}
```

### Review Screen (Dynamic Content)
```cpp
void DisplayShotReview() {
  lcd_display_p_at(&g_lcd, 0, 0, kShotNum);
  g_lcd.print(g_review_shot + 1);
  lcd_print_p(&g_lcd, kClearLine);
  lcd_display_p_at(&g_lcd, 11, 0, kSplit);
  lcd_display_time_at(&g_lcd, 0, 1, g_shot_times[g_review_shot], 9);
}
```

---

## Performance Notes

- All helper functions are inline and have minimal overhead
- PROGMEM strings are handled efficiently
- No additional memory allocation
- Functions are optimized for 16x2 LCD displays

---

## Troubleshooting

**Issue:** Text doesn't appear at expected position
- **Solution:** Check column/row values (0-indexed, max 15 for col, 1 for row)

**Issue:** Time displays incorrectly
- **Solution:** Ensure using `lcd_display_time_at()` not `lcd_display_at()`

**Issue:** PROGMEM string doesn't display
- **Solution:** Use `lcd_display_p_at()` for PROGMEM, `lcd_display_f_at()` for F()

**Issue:** Status display shows wrong value
- **Solution:** Check boolean logic and kEnabled/kDisabled order

