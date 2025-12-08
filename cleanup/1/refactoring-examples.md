# Example: Before & After Refactoring

## Example 1: IncreaseTime/DecreaseTime Functions

### BEFORE (135 lines total)
```cpp
void IncreaseTime() {
  DEBUG_PRINT(F("Increase time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  switch (g_par_cursor) {
    case 1: // milliseconds
      if (g_par_times[g_current_par] == 5999999) {
        break;
      }
      else {
        g_par_times[g_current_par]++;
      }
      break;
    case 2: // hundreds milliseconds
      if (g_par_times[g_current_par] >= 5999990) {
        break;
      }
      else {
        g_par_times[g_current_par] += 10;
      }
      break;
    // ... 5 more cases ...
  }
  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], 9);
}

void DecreaseTime() {
  DEBUG_PRINT(F("Decrease time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  switch (g_par_cursor) {
    case 1:
      if (g_par_times[g_current_par] < 1) {
        break;
      }
      else {
        g_par_times[g_current_par]--;
      }
      break;
    // ... 6 more nearly identical cases ...
  }
  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], 9);
}
```

### AFTER (70 lines total - including helper)
```cpp
// Helper function consolidates both increase and decrease logic
void AdjustParTime(int8_t direction) {
  unsigned long increment = 1;
  unsigned long max_val = 5999999;
  unsigned long min_val = 0;
  
  switch (g_par_cursor) {
    case 1: // milliseconds
      increment = 1; max_val = 5999999; min_val = 1; break;
    case 2: // hundreds milliseconds
      increment = 10; max_val = 5999990; min_val = 10; break;
    // ... other cases ...
  }
  
  if (direction > 0) {
    if (g_par_times[g_current_par] < max_val) {
      g_par_times[g_current_par] += increment;
    }
  }
  else {
    if (g_par_times[g_current_par] >= min_val) {
      g_par_times[g_current_par] -= increment;
    }
  }
  
  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], 9);
}

void IncreaseTime() {
  DEBUG_PRINT(F("Increase time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  AdjustParTime(1);
}

void DecreaseTime() {
  DEBUG_PRINT(F("Decrease time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  AdjustParTime(-1);
}
```

**Result:** 65 lines saved, single source of truth for increment values

---

## Example 2: NextShot/PreviousShot Functions

### BEFORE (60 lines)
```cpp
void NextShot() {
  DEBUG_PRINTLN(F("NextShot()"), 0);
  DEBUG_PRINTLN(g_current_state, 0);
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kShotNum);
  if (g_review_shot == g_current_shot) {
    g_review_shot = 0;
  } else g_review_shot++;
  g_lcd.print(g_review_shot + 1);
  lcd_print_p(&g_lcd, kClearLine);
  g_lcd.setCursor(11, 0);
  lcd_print_p(&g_lcd, kSplit);
  g_lcd.setCursor(0, 1);
  lcd_print_time(&g_lcd, g_shot_times[g_review_shot], 9);
  // ... 8 more lines ...
}

void PreviousShot() {
  // Nearly identical 30 lines of code
}
```

### AFTER (35 lines)
```cpp
void DisplayShotReview() {
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kShotNum);
  g_lcd.print(g_review_shot + 1);
  // ... display logic once ...
}

void NextShot() {
  DEBUG_PRINTLN(F("NextShot()"), 0);
  DEBUG_PRINTLN(g_current_state, 0);
  if (g_review_shot == g_current_shot) {
    g_review_shot = 0;
  } else g_review_shot++;
  DisplayShotReview();
}

void PreviousShot() {
  DEBUG_PRINTLN(F("PreviousShot()"), 0);
  DEBUG_PRINTLN(g_current_state, 0);
  DEBUG_PRINTLN(g_current_state, 0);
  if (g_review_shot == 0) {
    g_review_shot = g_current_shot;
  } else g_review_shot--;
  DisplayShotReview();
}
```

**Result:** 25 lines saved, display logic centralized

