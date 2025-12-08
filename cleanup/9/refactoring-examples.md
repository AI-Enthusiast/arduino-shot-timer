# Step 9: Before and After Examples

## Example 1: AdjustParTime() Function

### BEFORE (58 lines)

```cpp
void AdjustParTime(int8_t direction) {
  // direction: 1 for increase, -1 for decrease
  unsigned long increment = PAR_TIME_INCREMENT_1MS;
  unsigned long max_val = PAR_TIME_MAX;
  unsigned long min_val = 0;

  switch (g_par_cursor) {
    case PAR_CURSOR_1MS: // milliseconds
      increment = PAR_TIME_INCREMENT_1MS;
      max_val = PAR_TIME_MAX_1MS;
      min_val = PAR_TIME_MIN_1MS;
      break;
    case PAR_CURSOR_10MS: // tens milliseconds
      increment = PAR_TIME_INCREMENT_10MS;
      max_val = PAR_TIME_MAX_10MS;
      min_val = PAR_TIME_MIN_10MS;
      break;
    case PAR_CURSOR_100MS: // hundreds milliseconds
      increment = PAR_TIME_INCREMENT_100MS;
      max_val = PAR_TIME_MAX_100MS;
      min_val = PAR_TIME_MIN_100MS;
      break;
    case PAR_CURSOR_1S: // seconds
      increment = PAR_TIME_INCREMENT_1S;
      max_val = PAR_TIME_MAX_1S;
      min_val = PAR_TIME_MIN_1S;
      break;
    case PAR_CURSOR_10S: // ten seconds
      increment = PAR_TIME_INCREMENT_10S;
      max_val = PAR_TIME_MAX_10S;
      min_val = PAR_TIME_MIN_10S;
      break;
    case PAR_CURSOR_1MIN: // minutes
      increment = PAR_TIME_INCREMENT_1MIN;
      max_val = PAR_TIME_MAX_1MIN;
      min_val = PAR_TIME_MIN_1MIN;
      break;
    case PAR_CURSOR_10MIN: // 10 minutes
      increment = PAR_TIME_INCREMENT_10MIN;
      max_val = PAR_TIME_MAX_10MIN;
      min_val = PAR_TIME_MIN_10MIN;
      break;
  }

  if (direction > 0) {
    // Increase
    if (g_par_times[g_current_par] < max_val) {
      g_par_times[g_current_par] += increment;
    }
  }
  else {
    // Decrease
    if (g_par_times[g_current_par] >= min_val) {
      g_par_times[g_current_par] -= increment;
    }
  }

  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
}
```

### AFTER (27 lines + lookup table)

**Lookup Table Definition (one-time, reusable):**
```cpp
struct ParTimeBoundaries {
  unsigned long increment;
  unsigned long max_val;
  unsigned long min_val;
};

const ParTimeBoundaries kParTimeBoundaries[] PROGMEM = {
  {0, 0, 0},  // Index 0 - unused
  {PAR_TIME_INCREMENT_1MS, PAR_TIME_MAX_1MS, PAR_TIME_MIN_1MS},       // PAR_CURSOR_1MS
  {PAR_TIME_INCREMENT_10MS, PAR_TIME_MAX_10MS, PAR_TIME_MIN_10MS},    // PAR_CURSOR_10MS
  {PAR_TIME_INCREMENT_100MS, PAR_TIME_MAX_100MS, PAR_TIME_MIN_100MS}, // PAR_CURSOR_100MS
  {PAR_TIME_INCREMENT_1S, PAR_TIME_MAX_1S, PAR_TIME_MIN_1S},          // PAR_CURSOR_1S
  {PAR_TIME_INCREMENT_10S, PAR_TIME_MAX_10S, PAR_TIME_MIN_10S},       // PAR_CURSOR_10S
  {PAR_TIME_INCREMENT_1MIN, PAR_TIME_MAX_1MIN, PAR_TIME_MIN_1MIN},    // PAR_CURSOR_1MIN
  {PAR_TIME_INCREMENT_10MIN, PAR_TIME_MAX_10MIN, PAR_TIME_MIN_10MIN}  // PAR_CURSOR_10MIN
};
```

**Simplified Function:**
```cpp
void AdjustParTime(int8_t direction) {
  // direction: 1 for increase, -1 for decrease
  
  // Lookup boundaries from table
  unsigned long increment = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].increment);
  unsigned long max_val = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].max_val);
  unsigned long min_val = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].min_val);

  if (direction > 0) {
    // Increase
    if (g_par_times[g_current_par] < max_val) {
      g_par_times[g_current_par] += increment;
    }
  }
  else {
    // Decrease
    if (g_par_times[g_current_par] >= min_val) {
      g_par_times[g_current_par] -= increment;
    }
  }

  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
}
```

**Key Improvements:**
- ❌ Eliminated 7-case switch statement (49 lines)
- ✅ Simple 3-line lookup
- ✅ Data stored in flash memory (PROGMEM)
- ✅ Easy to add new cursor positions (just add to table)
- ✅ Single source of truth for boundaries

---

## Example 2: CycleSampleWindowAndDisplay() Function

### BEFORE (24 lines)

```cpp
void CycleSampleWindowAndDisplay(int8_t step) {
  if (step > 0) {
    if (g_sample_window == SAMPLE_WINDOW_MAX) {
      g_sample_window = SAMPLE_WINDOW_MIN;
    }
    else {
      g_sample_window += SAMPLE_WINDOW_STEP;
    }
  }
  else {
    if (g_sample_window == SAMPLE_WINDOW_MIN) {
      g_sample_window = SAMPLE_WINDOW_MAX;
    }
    else {
      g_sample_window -= SAMPLE_WINDOW_STEP;
    }
  }

  g_lcd.setCursor(0, 1);
  g_lcd.print(g_sample_window);
  lcd_print_p(&g_lcd, kMS);
  lcd_print_p(&g_lcd, kClearLine);
}
```

### AFTER (18 lines)

```cpp
void CycleSampleWindowAndDisplay(int8_t step) {
  int16_t new_value = g_sample_window + (step * SAMPLE_WINDOW_STEP);
  
  // Wrap around at boundaries
  if (new_value > SAMPLE_WINDOW_MAX) {
    g_sample_window = SAMPLE_WINDOW_MIN;
  }
  else if (new_value < SAMPLE_WINDOW_MIN) {
    g_sample_window = SAMPLE_WINDOW_MAX;
  }
  else {
    g_sample_window = new_value;
  }

  g_lcd.setCursor(0, 1);
  g_lcd.print(g_sample_window);
  lcd_print_p(&g_lcd, kMS);
  lcd_print_p(&g_lcd, kClearLine);
}
```

**Key Improvements:**
- ❌ Eliminated nested if-else for increase/decrease (8 lines)
- ✅ Calculate first, validate second approach
- ✅ More readable logic flow
- ✅ Easier to understand at a glance

---

## Memory Comparison

### Before
- Switch statement compiled into jump table or if-else chain
- Code stored in flash memory
- Estimated size: ~120 bytes

### After
- Lookup table in PROGMEM: 56 bytes
- Simplified code: ~60 bytes
- **Net savings: ~4 bytes** + improved maintainability

---

## Pattern Summary

### When to Apply This Pattern

Use lookup tables when you have:
- ✅ Switch statements with repetitive case bodies
- ✅ Cases that only differ in constant values
- ✅ 3+ cases (overhead of table justified)
- ✅ Data that doesn't change at runtime

### Steps to Apply

1. **Identify the pattern** - What varies between cases?
2. **Create a struct** - Define structure for varying data
3. **Build lookup table** - Array indexed by switch variable
4. **Use PROGMEM** - Store in flash to save RAM
5. **Replace switch** - Use array lookup with pgm_read_*

### PROGMEM Access Functions

```cpp
pgm_read_byte(&array[index])   // for uint8_t
pgm_read_word(&array[index])   // for uint16_t
pgm_read_dword(&array[index])  // for uint32_t (unsigned long)
pgm_read_ptr(&array[index])    // for pointers
```

---

## Conclusion

The lookup table pattern provides:
- **Better Code Density** - More logic per line
- **Improved Maintainability** - One place to change values
- **Memory Efficiency** - Data in flash instead of code
- **Extensibility** - Easy to add new entries
- **Readability** - Intent is clearer

This is a win-win refactoring that improves both code quality and resource usage! 🎉

