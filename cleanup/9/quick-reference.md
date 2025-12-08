# Cleanup Step 9: Boundary Checking - Quick Reference

## Key Changes

### 1. Par Time Boundaries Lookup Table

Instead of switch statements, use a lookup table:

```cpp
struct ParTimeBoundaries {
  unsigned long increment;
  unsigned long max_val;
  unsigned long min_val;
};

const ParTimeBoundaries kParTimeBoundaries[] PROGMEM = {
  {0, 0, 0},  // Index 0 - unused
  {PAR_TIME_INCREMENT_1MS, PAR_TIME_MAX_1MS, PAR_TIME_MIN_1MS},
  {PAR_TIME_INCREMENT_10MS, PAR_TIME_MAX_10MS, PAR_TIME_MIN_10MS},
  // ... etc
};
```

### 2. Accessing PROGMEM Data

Use `pgm_read_dword()` to read from the lookup table:

```cpp
unsigned long increment = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].increment);
unsigned long max_val = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].max_val);
unsigned long min_val = pgm_read_dword(&kParTimeBoundaries[g_par_cursor].min_val);
```

### 3. Simplified Boundary Logic

Instead of:
```cpp
if (step > 0) {
  if (value == MAX) {
    value = MIN;
  } else {
    value += STEP;
  }
} else {
  if (value == MIN) {
    value = MAX;
  } else {
    value -= STEP;
  }
}
```

Use:
```cpp
int16_t new_value = value + (step * STEP_SIZE);

if (new_value > MAX) {
  value = MIN;
} else if (new_value < MIN) {
  value = MAX;
} else {
  value = new_value;
}
```

## Benefits

1. **Reduced Code Size:** ~37 lines of repetitive code eliminated
2. **Better Memory Usage:** Lookup table stored in flash (PROGMEM) instead of code
3. **Easier Maintenance:** Single place to modify boundary values
4. **Simpler Logic:** Less nesting and branching
5. **Extensibility:** Easy to add new cursor positions

## Pattern to Follow

When you have repetitive switch statements setting similar values:

1. **Identify the pattern:** What varies between cases? (increment, max, min)
2. **Create a struct:** Define a structure to hold the varying values
3. **Build a lookup table:** Array of structs indexed by the switch variable
4. **Use PROGMEM:** Store in flash memory to save RAM
5. **Access with pgm_read_*:** Use appropriate PROGMEM read function

## Functions Simplified

- `AdjustParTime()` - Uses lookup table for boundaries
- `CycleSampleWindowAndDisplay()` - Simplified conditional logic
- `IncreaseTime()` / `DecreaseTime()` - Benefit from `AdjustParTime()` simplification

## Functions Already Well-Factored

- `CycleIntValueAndDisplay()` - Generic helper, no improvement needed
- `CycleValueAndDisplay()` - Generic helper, no improvement needed

