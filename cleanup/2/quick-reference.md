# Button Handler Quick Reference

## Quick Start Guide for Developers

This guide helps you quickly understand and work with the refactored button handling system.

---

## Architecture Overview

```
Button Press → ButtonListener() → Mapping Table → Handler Function → Action
```

**Example Flow:**
1. User presses UP button in SETDELAY state
2. ButtonListener reads button state
3. Looks up `setdelay_map[]` table
4. Finds `{BUTTON_UP, HandleIncreaseDelay}`
5. Calls `HandleIncreaseDelay()`
6. Delay value increases

---

## How to Add a New Button Action

### Step 1: Create Handler Function
```cpp
void HandleMyNewAction() {
  DEBUG_PRINTLN(F("UP/MyNewAction()"), 0);
  MyNewAction();  // Call your actual function
}
```

### Step 2: Add to Mapping Table
```cpp
static const ButtonMapping mystate_map[] = {
  {BUTTON_SELECT, HandleSettingsSelect},
  {BUTTON_UP, HandleMyNewAction},  // ← Add here
  {0, NULL}
};
```

### Step 3: Done!
No need to modify the dispatch logic.

---

## How to Add a New State

### Step 1: Create Handler Functions (if needed)
```cpp
void HandleNewStateSelect() {
  DEBUG_PRINTLN(F("SELECT/NewAction"), 0);
  DoNewAction();
}
```

### Step 2: Create Mapping Table
```cpp
static const ButtonMapping newstate_map[] = {
  {BUTTON_SELECT, HandleNewStateSelect},
  {BUTTON_LEFT, HandleSettingsLeft},      // Reuse existing
  {BUTTON_DOWN, HandleSomeDownAction},
  {BUTTON_UP, HandleSomeUpAction},
  {0, NULL}  // Don't forget sentinel!
};
```

### Step 3: Add to State Switch
In `ButtonListener()`, add:
```cpp
switch (*p_state) {
  // ... existing cases ...
  case NEWSTATE: current_map = newstate_map; break;
}
```

---

## How to Modify Existing Behavior

### Option 1: Modify Handler Function
**Use when:** You want to change what a button does everywhere

```cpp
void HandleSettingsSelect() {
  DEBUG_PRINTLN(F("SELECT/SELECT"), 0);
  // Add your new behavior here
  tm.select();
  // Or modify existing behavior
}
```

### Option 2: Create New Handler and Remap
**Use when:** You want to change behavior in one state only

```cpp
// New handler
void HandleDelaySelectCustom() {
  DEBUG_PRINTLN(F("SELECT/Custom"), 0);
  CustomBehavior();
}

// Update mapping
static const ButtonMapping setdelay_map[] = {
  {BUTTON_SELECT, HandleDelaySelectCustom},  // Changed
  // ... rest unchanged ...
};
```

---

## Common Patterns

### Pattern 1: Shared Handlers
Multiple states use the same handlers:

```cpp
// SETDELAY, SETBEEP, SETSENS, SETECHO all share:
{BUTTON_SELECT, HandleSettingsSelect}
{BUTTON_LEFT, HandleSettingsLeft}
```

**Benefit:** Change once, affects all states

### Pattern 2: State-Specific Handlers
Each state has unique handlers:

```cpp
// MENU state has unique handlers:
{BUTTON_DOWN, HandleMenuDown}   // Calls tm.next()
{BUTTON_UP, HandleMenuUp}       // Calls tm.prev()
```

### Pattern 3: Special Cases
Some states need custom logic:

```cpp
case TIMER:       
  // Special case: TIMER only handles SELECT
  if (new_button & BUTTON_SELECT) HandleTimerSelect();
  return;  // Skip normal dispatch
```

---

## Debugging Tips

### Finding What a Button Does
1. Look at the state (e.g., SETDELAY)
2. Find the mapping table (`setdelay_map[]`)
3. Look up the button (`BUTTON_UP`)
4. See the handler (`HandleIncreaseDelay`)
5. Find the handler function

**Example:**
```cpp
// Question: What does UP do in SETDELAY?
static const ButtonMapping setdelay_map[] = {
  // ...
  {BUTTON_UP, HandleIncreaseDelay},  // ← Answer!
  // ...
};
```

### Adding Debug Logging
Add logging to the dispatch loop:

```cpp
for (int i = 0; current_map[i].action != NULL; i++) {
  if (new_button & current_map[i].button) {
    DEBUG_PRINT(F("Executing handler for button: "));
    DEBUG_PRINTLN(current_map[i].button, HEX);
    current_map[i].action();
    break;
  }
}
```

### Testing a Handler
Each handler can be called directly for testing:

```cpp
void test_increase_delay() {
  uint8_t before = g_start_delay;
  HandleIncreaseDelay();
  assert(g_start_delay > before);
}
```

---

## State-Button Mapping Reference

| State | SELECT | RIGHT | LEFT | DOWN | UP |
|-------|--------|-------|------|------|-----|
| MENU | tm.select() | tm.select() | tm.back() | tm.next() | tm.prev() |
| TIMER | StopTimer() | - | - | - | - |
| REVIEW | tm.select() | RateOfFire() | NextShot() | PreviousShot() | NextShot() |
| SETPARSTATE | tm.select() | - | tm.select() | ToggleParState() | ToggleParState() |
| SETPARTIMES | EditPar() | - | tm.select() | ParDown() | ParUp() |
| SETINDPAR | EditPar() | RightCursor() | LeftCursor() | DecreaseTime() | IncreaseTime() |
| SETDELAY | tm.select() | - | tm.select() | DecreaseDelay() | IncreaseDelay() |
| SETROFDRAW | tm.select() | - | tm.select() | ToggleIncludeDraw() | ToggleIncludeDraw() |
| SETBEEP | tm.select() | - | tm.select() | DecreaseBeepVol() | IncreaseBeepVol() |
| SETSENS | tm.select() | - | tm.select() | DecreaseSensitivity() | IncreaseSensitivity() |
| SETECHO | tm.select() | - | tm.select() | DecreaseEchoProtect() | IncreaseEchoProtect() |

---

## Handler Function Reference

### Menu Handlers
- `HandleMenuSelect()` - Select menu item
- `HandleMenuRight()` - Select menu item (alternate)
- `HandleMenuLeft()` - Go back in menu
- `HandleMenuDown()` - Move to next menu item
- `HandleMenuUp()` - Move to previous menu item

### Timer Handlers
- `HandleTimerSelect()` - Stop timer

### Review Handlers
- `HandleReviewSelect()` - Exit review
- `HandleReviewRight()` - Show rate of fire
- `HandleReviewLeft()` - Previous shot (via g_review_shot--)
- `HandleReviewDown()` - Previous shot
- `HandleReviewUp()` - Next shot

### Par State Handlers
- `HandleParStateSelect()` - Confirm selection
- `HandleParStateLeft()` - Go back
- `HandleToggleParStateDown()` - Toggle par enabled/disabled
- `HandleToggleParStateUp()` - Toggle par enabled/disabled

### Par Times Handlers
- `HandleParTimesSelect()` - Edit selected par time
- `HandleParTimesLeft()` - Go back
- `HandleParDown()` - Select previous par time
- `HandleParUp()` - Select next par time

### Individual Par Handlers
- `HandleIndParSelect()` - Finish editing
- `HandleRightCursor()` - Move cursor right
- `HandleLeftCursor()` - Move cursor left
- `HandleDecreaseTime()` - Decrease value at cursor
- `HandleIncreaseTime()` - Increase value at cursor

### Settings Handlers (Shared)
- `HandleSettingsSelect()` - Confirm and exit
- `HandleSettingsLeft()` - Cancel and exit

### Delay Handlers
- `HandleDecreaseDelay()` - Decrease start delay
- `HandleIncreaseDelay()` - Increase start delay

### ROF Draw Handlers
- `HandleToggleIncludeDrawDown()` - Toggle include draw
- `HandleToggleIncludeDrawUp()` - Toggle include draw

### Beep Handlers
- `HandleDecreaseBeepVol()` - Decrease beep volume
- `HandleIncreaseBeepVol()` - Increase beep volume

### Sensitivity Handlers
- `HandleDecreaseSensitivity()` - Decrease microphone sensitivity
- `HandleIncreaseSensitivity()` - Increase microphone sensitivity

### Echo Protection Handlers
- `HandleDecreaseEchoProtect()` - Decrease echo protection time
- `HandleIncreaseEchoProtect()` - Increase echo protection time

---

## Advanced: Dynamic Button Mapping

### Future Enhancement: User-Configurable Buttons

With this architecture, you could add:

```cpp
// Load custom mapping from EEPROM
ButtonMapping custom_menu_map[6];
LoadButtonMappingFromEEPROM(MENU, custom_menu_map);

// Use custom or default
const ButtonMapping* current_map = 
  user_has_custom_mapping ? custom_menu_map : menu_map;
```

### Future Enhancement: Button Combinations

```cpp
// Check for combos before normal dispatch
if ((new_button & BUTTON_UP) && (new_button & BUTTON_DOWN)) {
  HandleResetToDefaults();
  return;
}
```

### Future Enhancement: Long Press

```cpp
// Track button hold time
if (button_held_for > LONG_PRESS_THRESHOLD) {
  HandleLongPress();
} else {
  // Normal dispatch
}
```

---

## Memory Considerations

### Static Tables
All mapping tables are marked `static const`:
- Stored in program memory (Flash), not RAM
- No runtime initialization overhead
- Total size: ~250 bytes in Flash

### Function Pointers
- Each pointer is 2 bytes on AVR
- Stored in mapping tables (Flash)
- No RAM overhead

### Handler Functions
- Normal functions, stored in Flash
- Called via function pointer
- No additional memory cost

---

## Best Practices

### DO ✅
- Keep handler functions small and focused
- Reuse handlers across states when possible
- Use descriptive handler names
- Keep mapping tables organized by state
- Add NULL sentinel at end of tables
- Document complex button interactions

### DON'T ❌
- Don't put complex logic in handlers (call other functions)
- Don't forget NULL sentinel in mapping tables
- Don't modify dispatch loop unless necessary
- Don't duplicate handler functions unnecessarily
- Don't hardcode button values (use BUTTON_* constants)

---

## Troubleshooting

### Button Not Responding
1. Check if handler is in mapping table
2. Verify button constant is correct
3. Check if handler function exists
4. Add debug logging to handler

### Wrong Action Triggered
1. Verify button constant in mapping
2. Check for duplicate entries in table
3. Verify handler function logic
4. Check state variable value

### All Buttons Not Working in State
1. Verify state case in switch statement
2. Check if mapping table pointer is set
3. Verify table has NULL sentinel
4. Check dispatch loop runs

---

## Quick Examples

### Example 1: Add Volume Boost Button
```cpp
// 1. Create handler
void HandleBoostVolume() {
  DEBUG_PRINTLN(F("RIGHT/BoostVolume()"), 0);
  g_beep_volume = 255;  // Max volume
}

// 2. Add to setbeep_map
static const ButtonMapping setbeep_map[] = {
  {BUTTON_SELECT, HandleSettingsSelect},
  {BUTTON_RIGHT, HandleBoostVolume},  // NEW
  {BUTTON_LEFT, HandleSettingsLeft},
  {BUTTON_DOWN, HandleDecreaseBeepVol},
  {BUTTON_UP, HandleIncreaseBeepVol},
  {0, NULL}
};
```

### Example 2: Change Menu Navigation
```cpp
// Change what UP/DOWN do in menu
static const ButtonMapping menu_map[] = {
  {BUTTON_SELECT, HandleMenuSelect},
  {BUTTON_RIGHT, HandleMenuRight},
  {BUTTON_LEFT, HandleMenuLeft},
  {BUTTON_DOWN, HandleMenuUp},    // Swapped!
  {BUTTON_UP, HandleMenuDown},    // Swapped!
  {0, NULL}
};
```

### Example 3: Add Special TIMER Behavior
```cpp
// In ButtonListener(), TIMER case:
case TIMER:       
  if (new_button & BUTTON_SELECT) HandleTimerSelect();
  if (new_button & BUTTON_UP) HandleTimerPause();     // NEW
  if (new_button & BUTTON_DOWN) HandleTimerResume();  // NEW
  return;
```

---

## Summary

The refactored button handling system provides:
- ✅ Clear separation of concerns
- ✅ Easy to understand and modify
- ✅ Highly extensible
- ✅ Minimal memory overhead
- ✅ Consistent patterns throughout

**Key takeaway:** To understand what a button does, just look at the mapping table!

---

**Last Updated:** December 7, 2025  
**Version:** 2.0 (Post-refactoring)

