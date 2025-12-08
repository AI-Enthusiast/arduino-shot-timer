# Cleanup Step 4: Quick Reference

## State Management Pattern

### How to Add a New State

1. **Add to enum**
```cpp
enum ProgramState {
  // ... existing states ...
  MYNEWSTATE  // Add your new state
} g_current_state;
```

2. **Declare handlers**
```cpp
void EnterMyNewState();
void ExitMyNewState();
```

3. **Add to TransitionToState()**
```cpp
switch (g_current_state) {
  // ... existing cases ...
  case MYNEWSTATE: ExitMyNewState(); break;
}

switch (new_state) {
  // ... existing cases ...
  case MYNEWSTATE: EnterMyNewState(); break;
}
```

4. **Implement handlers**
```cpp
void EnterMyNewState() {
  // Set up UI, initialize variables
  g_lcd.clear();
  g_lcd.print(F("My State"));
}

void ExitMyNewState() {
  // Save settings, cleanup
  g_my_setting_e = g_my_value;
}
```

5. **Use in code**
```cpp
void on_my_menu_selected(MenuItem* p_menu_item) {
  if(!IsInState(MYNEWSTATE)) {
    TransitionToState(MYNEWSTATE);
  } else {
    TransitionToState(MENU);
  }
}
```

---

## State Management Functions

### TransitionToState(ProgramState new_state)
**Use:** Change from current state to a new state
```cpp
TransitionToState(TIMER);
TransitionToState(MENU);
```

### IsInState(ProgramState state)
**Use:** Check if currently in a specific state
```cpp
if(IsInState(MENU)) {
  RenderMenu();
}
```

### IsInSettingsState()
**Use:** Check if in any settings state
```cpp
if(IsInSettingsState()) {
  // Common settings logic
}
```

### IsInParState()
**Use:** Check if in any par time state
```cpp
if(IsInParState()) {
  // Common par time logic
}
```

---

## State Lifecycle

```
┌─────────────────────────────────────────┐
│  TransitionToState(NEW_STATE) called   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  Early return if already in NEW_STATE   │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  Log transition: OLD_STATE → NEW_STATE  │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  Call ExitOldState()                     │
│  - Save settings                          │
│  - Reset UI elements                      │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  Update: g_current_state = NEW_STATE    │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  Call EnterNewState()                    │
│  - Initialize UI                          │
│  - Set up state variables                 │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│  State transition complete               │
└──────────────────────────────────────────┘
```

---

## All Program States

| State | Purpose | Entry Handler | Exit Handler |
|-------|---------|--------------|--------------|
| `MENU` | Menu navigation | Sets backlight, renders menu | None |
| `TIMER` | Timer running | Handled by start callback | None |
| `REVIEW` | Shot review | Handled by review callback | None |
| `SETPARSTATE` | Toggle par on/off | Shows enabled/disabled | None |
| `SETPARTIMES` | Navigate par times | Handled by par callback | None |
| `SETINDPAR` | Edit individual par | Shows edit UI, green light | Resets backlight |
| `SETDELAY` | Set start delay | Shows delay UI | Saves to EEPROM |
| `SETROFDRAW` | Set include draw | Shows draw UI | Saves to EEPROM |
| `SETBEEP` | Set beep volume | Shows volume UI | Saves to EEPROM |
| `SETSENS` | Set sensitivity | Shows sensitivity UI | Saves, updates threshold |
| `SETECHO` | Set echo protect | Shows echo UI | Saves to EEPROM |

---

## Common Patterns

### Simple Toggle State
```cpp
void on_menu_item_selected(MenuItem* p_menu_item) {
  if(!IsInState(MYSTATE)) {
    TransitionToState(MYSTATE);
  } else {
    TransitionToState(MENU);
  }
}
```

### State with Custom Setup
```cpp
void on_menu_item_selected(MenuItem* p_menu_item) {
  if(!IsInState(MYSTATE)) {
    // Do custom setup before transition
    g_custom_var = calculate_something();
    
    // Then transition
    TransitionToState(MYSTATE);
  } else {
    TransitionToState(MENU);
  }
}
```

### Conditional State Transition
```cpp
void some_handler() {
  if(condition_met && !IsInState(TARGET)) {
    TransitionToState(TARGET);
  }
}
```

---

## Best Practices

### ✅ DO
- Use `TransitionToState()` for ALL state changes
- Use `IsInState()` for state checks
- Put UI initialization in entry handlers
- Put settings saves in exit handlers
- Log important actions in handlers

### ❌ DON'T
- Directly assign `g_current_state = ...`
- Check state with `g_current_state == ...` (use `IsInState()`)
- Mix state transition with business logic
- Forget to add new states to TransitionToState() switch
- Duplicate UI setup code across functions

---

## Debugging State Issues

### Enable Debug Output
State transitions automatically log:
```
State transition: 0 -> 6
Enter SETDELAY state
```

### Common Issues
1. **State not changing**: Check if TransitionToState() is called
2. **UI not updating**: Check entry handler implementation
3. **Settings not saving**: Check exit handler implementation
4. **Wrong state after action**: Check state machine logic in TransitionToState()

---

## Migration Checklist

When migrating old state management code:

- [ ] Find direct `g_current_state = STATE` assignments
- [ ] Replace with `TransitionToState(STATE)` calls
- [ ] Extract UI setup to entry handler
- [ ] Extract settings save to exit handler
- [ ] Replace `g_current_state == STATE` with `IsInState(STATE)`
- [ ] Test state transitions work correctly
- [ ] Verify settings persist
- [ ] Check debug output logs

---

## Performance Notes

- State transitions are lightweight (O(1) switch statements)
- Entry/exit handlers only called on actual state changes
- No dynamic memory allocation
- Minimal stack usage
- Arduino-friendly implementation

---

## Related Files

- **Implementation**: `ShotTimer.ino` (lines 306-420)
- **Completion Report**: `cleanup/4/completion-report.md`
- **Refactoring Examples**: `cleanup/4/refactoring-examples.md`

