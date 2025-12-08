# LCD Operations Consolidation - Refactoring Examples

This document shows specific before/after examples of the LCD operations refactoring performed in cleanup step 5.

## Example 1: State Entry with Enable/Disable Display

### Before (EnterParStateState)
```cpp
void EnterParStateState() {
  DEBUG_PRINTLN(F("Enter SETPARSTATE state"), 0);
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  g_lcd.print(F("Par Times"));
  g_lcd.setCursor(0, 1);
  if (g_par_enabled == false) {
    lcd_print_p(&g_lcd, kDisabled);
  } else {
    lcd_print_p(&g_lcd, kEnabled);
  }
}
```

### After (EnterParStateState)
```cpp
void EnterParStateState() {
  DEBUG_PRINTLN(F("Enter SETPARSTATE state"), 0);
  lcd_display_header(&g_lcd, F("Par Times"), F(""));
  lcd_display_enabled_status(&g_lcd, g_par_enabled, kEnabled, kDisabled);
}
```

**Lines reduced:** 11 → 4 (63% reduction)  
**Benefits:**
- Eliminated repetitive clear/setCursor/print pattern
- Consolidated if/else logic into helper function
- Intent is immediately clear from function names

---

## Example 2: State Entry with Multiple Positioned Elements

### Before (EnterIndParState)
```cpp
void EnterIndParState() {
  DEBUG_PRINTLN(F("Enter SETINDPAR state"), 0);
  g_lcd.setBacklight(GREEN);
  g_lcd.setCursor(0, 0);
  g_lcd.print(F("Edit"));
  lcd_print_p(&g_lcd, kClearLine);
  g_lcd.setCursor(0, 1);
  g_lcd.print(F("P"));
  lcd_print(&g_lcd, g_current_par + 1, LCD_DISPLAY_WIDTH_2);
  g_lcd.setCursor(4, 1);
  if (g_current_par > 0) {
    lcd_print_p(&g_lcd, kPlus);
  } else {
    lcd_print_p(&g_lcd, kSpace);
  }
  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
  g_par_cursor = PAR_CURSOR_DEFAULT;
  LCDCursor();
}
```

### After (EnterIndParState)
```cpp
void EnterIndParState() {
  DEBUG_PRINTLN(F("Enter SETINDPAR state"), 0);
  g_lcd.setBacklight(GREEN);
  lcd_display_f_at(&g_lcd, 0, 0, F("Edit"));
  lcd_display_p_at(&g_lcd, 5, 0, kClearLine);
  lcd_display_f_at(&g_lcd, 0, 1, F("P"));
  lcd_display_at(&g_lcd, 1, 1, g_current_par + 1, LCD_DISPLAY_WIDTH_2);
  lcd_display_p_at(&g_lcd, 4, 1, (g_current_par > 0) ? kPlus : kSpace);
  lcd_display_time_at(&g_lcd, 5, 1, g_par_times[g_current_par], LCD_TIME_WIDTH);
  g_par_cursor = PAR_CURSOR_DEFAULT;
  LCDCursor();
}
```

**Lines reduced:** 20 → 11 (45% reduction)  
**Benefits:**
- Each positioned element is one function call instead of two
- Ternary operator replaces if/else for conditional display
- Clear column/row positions visible in function calls

---

## Example 3: Simple Two-Line Configuration Screen

### Before (EnterBeepState)
```cpp
void EnterBeepState() {
  DEBUG_PRINTLN(F("Enter SETBEEP state"), 0);
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  g_lcd.print(F("Beep Volume"));
  g_lcd.setCursor(0, 1);
  g_lcd.print(g_beep_vol);
}
```

### After (EnterBeepState)
```cpp
void EnterBeepState() {
  DEBUG_PRINTLN(F("Enter SETBEEP state"), 0);
  lcd_display_header(&g_lcd, F("Beep Volume"), F(""));
  lcd_display_int_at(&g_lcd, 0, 1, g_beep_vol);
}
```

**Lines reduced:** 7 → 4 (43% reduction)  
**Benefits:**
- Header pattern reduced to single call
- Integer display uses specialized function
- Less visual clutter

---

## Example 4: Menu Rendering

### Before (RenderMenu)
```cpp
void RenderMenu() {
  Menu const* kMenu = tm.get_current_menu();
  g_lcd.setBacklight(WHITE);
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kMenu->get_name());
  DEBUG_PRINT(F("Rendering Menu: "));
  DEBUG_PRINTLN_P(kMenu->get_name(),0);
  g_lcd.setCursor(0, 1);
  lcd_print_p(&g_lcd, kMenu->get_selected()->get_name());
  DEBUG_PRINT(F("Rendering Item: "));
  DEBUG_PRINTLN_P(kMenu->get_selected()->get_name(),0);
}
```

### After (RenderMenu)
```cpp
void RenderMenu() {
  Menu const* kMenu = tm.get_current_menu();
  g_lcd.setBacklight(WHITE);
  g_lcd.clear();
  lcd_display_p_at(&g_lcd, 0, 0, kMenu->get_name());
  DEBUG_PRINT(F("Rendering Menu: "));
  DEBUG_PRINTLN_P(kMenu->get_name(),0);
  lcd_display_p_at(&g_lcd, 0, 1, kMenu->get_selected()->get_name());
  DEBUG_PRINT(F("Rendering Item: "));
  DEBUG_PRINTLN_P(kMenu->get_selected()->get_name(),0);
}
```

**Lines reduced:** 13 → 11 (15% reduction)  
**Benefits:**
- setCursor + lcd_print_p combined into single calls
- Cleaner flow with positioned display functions

---

## Example 5: Recording Shot Times

### Before (RecordShot)
```cpp
void RecordShot() {
  g_shot_times[g_current_shot] = g_shot_chrono.elapsed();
  DEBUG_PRINT_P(kShotNum); DEBUG_PRINT(g_current_shot + 1); 
  DEBUG_PRINT(F(" - "));
  DEBUG_PRINT(F("\n"));
  g_lcd.setCursor(13, 0);
  lcd_print(&g_lcd, g_current_shot + 1, LCD_DISPLAY_WIDTH_3);
  g_lcd.setCursor(7, 1);
  lcd_print_time(&g_lcd, g_shot_times[g_current_shot], LCD_TIME_WIDTH);
  g_current_shot++;
  if (g_current_shot == kShotLimit) { 
    DEBUG_PRINTLN(F("Out of room for shots"),0);
    StopTimer(1);
  }
}
```

### After (RecordShot)
```cpp
void RecordShot() {
  g_shot_times[g_current_shot] = g_shot_chrono.elapsed();
  DEBUG_PRINT_P(kShotNum); DEBUG_PRINT(g_current_shot + 1); 
  DEBUG_PRINT(F(" - "));
  DEBUG_PRINT(F("\n"));
  lcd_display_at(&g_lcd, 13, 0, g_current_shot + 1, LCD_DISPLAY_WIDTH_3);
  lcd_display_time_at(&g_lcd, 7, 1, g_shot_times[g_current_shot], LCD_TIME_WIDTH);
  g_current_shot++;
  if (g_current_shot == kShotLimit) { 
    DEBUG_PRINTLN(F("Out of room for shots"),0);
    StopTimer(1);
  }
}
```

**Lines reduced:** 14 → 12 (14% reduction)  
**Benefits:**
- Positioned updates are clearer
- Time and number displays in one call each

---

## Example 6: Toggle Functions

### Before (ToggleParState)
```cpp
void ToggleParState() {
  g_par_enabled = !g_par_enabled;
  DEBUG_PRINT(F("g_current_state: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINT(F("Toggled Par to: "));DEBUG_PRINTLN(g_par_enabled, 0);
  g_lcd.setCursor(0, 1);
  if (g_par_enabled == false) {
    lcd_print_p(&g_lcd, kDisabled);
  }
  else {
    lcd_print_p(&g_lcd, kEnabled);
  }
}
```

### After (ToggleParState)
```cpp
void ToggleParState() {
  g_par_enabled = !g_par_enabled;
  DEBUG_PRINT(F("g_current_state: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINT(F("Toggled Par to: "));DEBUG_PRINTLN(g_par_enabled, 0);
  lcd_display_enabled_status(&g_lcd, g_par_enabled, kEnabled, kDisabled);
}
```

**Lines reduced:** 11 → 5 (55% reduction)  
**Benefits:**
- Eliminated if/else block
- Single function call expresses intent clearly
- Pattern is reusable across all toggle functions

---

## Example 7: Complex Multi-Element Display

### Before (on_menu_par_times_selected)
```cpp
void on_menu_par_times_selected(MenuItem* p_menu_item) {
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    DEBUG_PRINT(F("State before select: ")); DEBUG_PRINTLN(g_current_state,0);
  if(!IsInState(SETPARTIMES)){
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("<<"));
    g_lcd.setCursor(5, 0);
    g_lcd.print(F("Par"));
    g_lcd.setCursor(9, 0);
    lcd_print(&g_lcd, (g_current_par + 1), LCD_DISPLAY_WIDTH_2);
    g_lcd.setCursor(4, 1);
    if (g_current_par > 0) {
      lcd_print_p(&g_lcd, kPlus);
    }
    else {
      lcd_print_p(&g_lcd, kSpace);
    }
    lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    TransitionToState(SETPARTIMES);
  }
  else {
    TransitionToState(MENU);
  }
}
```

### After (on_menu_par_times_selected)
```cpp
void on_menu_par_times_selected(MenuItem* p_menu_item) {
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    DEBUG_PRINT(F("State before select: ")); DEBUG_PRINTLN(g_current_state,0);
  if(!IsInState(SETPARTIMES)){
    g_lcd.clear();
    lcd_display_f_at(&g_lcd, 0, 0, F("<<"));
    lcd_display_f_at(&g_lcd, 5, 0, F("Par"));
    lcd_display_at(&g_lcd, 9, 0, g_current_par + 1, LCD_DISPLAY_WIDTH_2);
    lcd_display_p_at(&g_lcd, 4, 1, (g_current_par > 0) ? kPlus : kSpace);
    lcd_display_time_at(&g_lcd, 5, 1, g_par_times[g_current_par], LCD_TIME_WIDTH);
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    TransitionToState(SETPARTIMES);
  }
  else {
    TransitionToState(MENU);
  }
}
```

**Lines reduced:** 25 → 15 (40% reduction)  
**Benefits:**
- Each element positioned and displayed in one call
- Ternary operator for conditional content
- Visual layout of screen elements is clearer

---

## Example 8: Display Helper Function

### Before (DisplayParInfo)
```cpp
void DisplayParInfo() {
  g_lcd.setCursor(9, 0);
  lcd_print(&g_lcd, (g_current_par + 1), 2);
  g_lcd.setCursor(4, 1);
  if (g_current_par > 0) {
    lcd_print_p(&g_lcd, kPlus);
  }
  else {
    lcd_print_p(&g_lcd, kSpace);
  }
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
}
```

### After (DisplayParInfo)
```cpp
void DisplayParInfo() {
  lcd_display_at(&g_lcd, 9, 0, g_current_par + 1, 2);
  lcd_display_p_at(&g_lcd, 4, 1, (g_current_par > 0) ? kPlus : kSpace);
  lcd_display_time_at(&g_lcd, 5, 1, g_par_times[g_current_par], LCD_TIME_WIDTH);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
}
```

**Lines reduced:** 13 → 5 (62% reduction)  
**Benefits:**
- Dramatically simplified
- Clear positioning of each element
- Easy to visualize screen layout

---

## Summary of Improvements

| Function | Before (lines) | After (lines) | Reduction |
|----------|---------------|--------------|-----------|
| EnterParStateState | 11 | 4 | 63% |
| EnterIndParState | 20 | 11 | 45% |
| EnterBeepState | 7 | 4 | 43% |
| EnterSensState | 7 | 4 | 43% |
| EnterEchoState | 7 | 4 | 43% |
| EnterRofDrawState | 11 | 4 | 63% |
| ToggleParState | 11 | 5 | 55% |
| ToggleIncludeDraw | 11 | 5 | 55% |
| on_menu_par_times_selected | 25 | 15 | 40% |
| DisplayParInfo | 13 | 5 | 62% |
| **Total** | **123** | **61** | **50%** |

## Key Patterns Consolidated

1. **Clear + Two-Line Header** → `lcd_display_header()`
2. **setCursor + print** → `lcd_display_*_at()`
3. **Enable/Disable if/else** → `lcd_display_enabled_status()`
4. **Conditional content** → Ternary operators with helpers
5. **Time display positioning** → `lcd_display_time_at()`

## Code Quality Metrics

- **Total lines reduced:** ~62 lines from refactored functions
- **Duplicate patterns eliminated:** 5+ major patterns
- **Average function size reduction:** 50%
- **New helper functions created:** 10
- **Functions refactored:** 19

## Conclusion

These refactoring examples demonstrate the significant improvement in code clarity, maintainability, and conciseness achieved through the LCD operations consolidation. The new helper functions provide a consistent, self-documenting API that makes the code easier to understand and modify.

