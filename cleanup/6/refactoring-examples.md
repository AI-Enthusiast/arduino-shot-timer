# Cleanup Step 6: Refactoring Examples

This document provides detailed before/after examples of the code cleanup performed.

## Example 1: Removing TODO Comments

### Before
```cpp
//////////////////////////////
// Instantiation //@TODO: should maybe have a settings object and timer object? 
//////////////////////////////
LightChrono g_shot_chrono;
```

### After
```cpp
//////////////////////////////
// Instantiation
//////////////////////////////
LightChrono g_shot_chrono;
```

**Rationale**: The TODO didn't provide actionable information and cluttered the code. If this refactoring is needed, it should be tracked in an issue tracker, not in the code.

---

## Example 2: Removing Commented DEBUG Lines

### Before
```cpp
void RunTimer(ProgramState* p_state, boolean* par_state) {
  //DEBUG_PRINTLN(*runState, 0);
  if (*p_state == TIMER)
  { 
    //DEBUG_PRINTLN(F("Enter Run Timer Mode."), 0);
    ShotListener();
    ParBeeps(par_state);
  }
}
```

### After
```cpp
void RunTimer(ProgramState* p_state, boolean* par_state) {
  if (*p_state == TIMER)
  { 
    ShotListener();
    ParBeeps(par_state);
  }
}
```

**Rationale**: These commented-out DEBUG statements were left over from debugging sessions. They're not needed because:
- The function is simple enough to understand without them
- Active DEBUG macros exist elsewhere for state tracking
- Commented code creates confusion about what's actually running

---

## Example 3: Removing Dead Code Blocks

### Before
```cpp
void on_menu_review_selected(MenuItem* p_menu_item) {
  if(!IsInState(REVIEW)){
    //DEBUG FOR LOOP - PRINT ALL SHOT TIMES IN THE STRING TO SERIAL
    // for (int t = 0; t < g_current_shot; t++) {
    //   DEBUG_PRINT_P(kShotNum);
    //   DEBUG_PRINT(t + 1);
    //   DEBUG_PRINT(F(" - "));
    //   DEBUG_PRINTLN(g_shot_times[t],0) // for DEBUG
    // }
    //END DEBUG 
    g_lcd.setBacklight(VIOLET);
    g_review_shot = g_current_shot;
    // ... rest of function
  }
}
```

### After
```cpp
void on_menu_review_selected(MenuItem* p_menu_item) {
  if(!IsInState(REVIEW)){
    g_lcd.setBacklight(VIOLET);
    g_review_shot = g_current_shot;
    // ... rest of function
  }
}
```

**Rationale**: This entire debug block was commented out and no longer needed. If shot times debugging is needed in the future, it can be re-implemented using the existing DEBUG macro system.

---

## Example 4: Removing Old Implementation Code

### Before
```cpp
g_additive_par += g_par_times[i]; // add the g_par_times together
//if (shotTimer.elapsed() <= (g_additive_par + (g_sample_window / 2)) 
//&& shotTimer.elapsed() >= (g_additive_par - g_sample_window / 2)){
int time_elapsed = g_shot_chrono.elapsed();
// Beep if the current time matches the parTime
// (within the boundaries of sample window) 
if (time_elapsed <= (g_additive_par + (g_sample_window / 2)) 
  && time_elapsed >= (g_additive_par - g_sample_window / 2)) {
```

### After
```cpp
g_additive_par += g_par_times[i]; // add the g_par_times together
int time_elapsed = g_shot_chrono.elapsed();
// Beep if the current time matches the parTime
// (within the boundaries of sample window) 
if (time_elapsed <= (g_additive_par + (g_sample_window / 2)) 
  && time_elapsed >= (g_additive_par - g_sample_window / 2)) {
```

**Rationale**: The commented code was an old implementation using `shotTimer` instead of the chrono object. The active code below it is the correct implementation, so the old version should be removed.

---

## Example 5: Improving Function Headers

### Before
```cpp
//////////////////////////////
// ParDown()
//////////////////////////////

void ParDown() {
  DEBUG_PRINTLN(F("ParDown()"), 0);
  if (g_current_par == 0) {
    g_current_par = kParLimit - 1;
  }
  else {
    g_current_par--;
  }
  DisplayParInfo();
}
```

### After
```cpp
//////////////////////////////
// ParDown - Navigate down in par times list
//////////////////////////////

void ParDown() {
  DEBUG_PRINTLN(F("ParDown()"), 0);
  if (g_current_par == 0) {
    g_current_par = kParLimit - 1;
  }
  else {
    g_current_par--;
  }
  DisplayParInfo();
}
```

**Rationale**: The header now explains what the function does rather than just repeating its name. This makes the code self-documenting.

---

## Example 6: More Descriptive Headers

### Before
```cpp
//////////////////////////////
// LCDCursor()
//////////////////////////////
//switch case for cursor position displayed on screen
void LCDCursor() {
```

### After
```cpp
//////////////////////////////
// LCDCursor - Display cursor at current position on screen
//////////////////////////////

void LCDCursor() {
```

**Rationale**: The inline comment has been incorporated into the header, making it more prominent and removing the need for a separate comment line.

---

## Summary of Improvements

| Category | Count | Impact |
|----------|-------|--------|
| TODO comments removed | 2 | Reduced clutter |
| Commented DEBUG lines removed | 6 | Clearer active code |
| Dead code blocks removed | 2 | Eliminated confusion |
| Function headers improved | 8 | Better documentation |
| **Total lines removed** | **18** | **Cleaner codebase** |

## Active Code Preserved

### DEBUG Macros Still Active ✅
```cpp
DEBUG_PRINT(F("State transition: "));
DEBUG_PRINTLN(g_current_state);
DEBUG_PRINTLN(F("Enter MENU state"), 0);
DEBUG_PRINTLN(F("Reviewing Shot: "));
// ... and many more
```

These are controlled by `#define DEBUG` and are compiled out when not needed.

### Meaningful Comments Still Present ✅
- Section headers
- Algorithm explanations  
- Hardware-specific notes
- Boundary condition comments
- Complex logic explanations

---

## Verification

### Code Quality Checks
- ✅ No compilation errors
- ✅ No warnings introduced
- ✅ All functionality preserved
- ✅ Active DEBUG macros intact
- ✅ Meaningful comments preserved

### Readability Improvements
- ✅ Reduced visual clutter
- ✅ Clearer code intent
- ✅ Better function documentation
- ✅ No ambiguity about what's active
- ✅ Professional appearance

