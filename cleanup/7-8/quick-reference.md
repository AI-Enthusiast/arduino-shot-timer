# Quick Reference: Function Organization in ShotTimer.ino

## Function Sections Overview

### 🏗️ STATE MANAGEMENT (Line ~356-598)
**Purpose**: Manage application state transitions and menu rendering
- `TransitionToState()` - Central state transition handler
- `IsInState()`, `IsInSettingsState()`, `IsInParState()` - State queries
- 11 `Enter*State()` functions - State entry handlers
- 11 `Exit*State()` functions - State exit handlers
- `RenderMenu()` - Display current menu

### 🎤 AUDIO INPUT AND SOUND DETECTION (Line ~615-650)
**Purpose**: Sample and process microphone input
- `SampleSound()` - Read mic and calculate peak-to-peak amplitude

### ⏱️ TIMER FUNCTIONS (Line ~652-765)
**Purpose**: Core shot timer functionality
- `on_menu_start_selected()` - Initialize and start timer
- `RunTimer()` - Main timer execution (called in loop)
- `ParBeeps()` - Trigger beeps at par times
- `StopTimer()` - End timer session
- `RecordShot()` - Log shot time to array

### 📊 SHOT REVIEW FUNCTIONS (Line ~769-885)
**Purpose**: Review recorded shots and calculate statistics
- `on_menu_review_selected()` - Enter review mode
- `DisplayShotReview()` - Show shot details
- `NextShot()`, `PreviousShot()` - Navigate shots
- `RateOfFire()` - Calculate and display ROF

### ⚙️ SETTINGS FUNCTIONS (Line ~887-1175)
**Purpose**: Configure timer behavior and hardware settings

#### Start Delay (Line ~887-960)
- `on_menu_start_delay_selected()` - Enter delay settings
- `CycleValueAndDisplay()` - Generic value cycling helper
- `IncreaseDelay()`, `DecreaseDelay()` - Adjust delay
- `StartDelay()` - Execute configured delay

#### Rate of Fire (Line ~961-987)
- `on_menu_rof_selected()` - Enter ROF settings
- `ToggleIncludeDraw()` - Include/exclude draw in ROF

#### Buzzer Volume (Line ~987-1040)
- `on_menu_buzzer_selected()` - Enter beep volume settings
- `CycleIntValueAndDisplay()` - Integer cycling helper
- `IncreaseBeepVol()`, `DecreaseBeepVol()` - Adjust volume

#### Sensitivity (Line ~1039-1075)
- `on_menu_sensitivity_selected()` - Enter sensitivity settings
- `IncreaseSensitivity()`, `DecreaseSensitivity()` - Adjust sensitivity
- `SensToThreshold()` - Convert sensitivity to threshold value

#### Echo Protection (Line ~1072-1175)
- `on_menu_echo_selected()` - Enter echo settings
- `CycleSampleWindowAndDisplay()` - Sample window helper
- `IncreaseEchoProtect()`, `DecreaseEchoProtect()` - Adjust window

### 🎯 PAR TIME FUNCTIONS (Line ~1177-1455)
**Purpose**: Manage and edit par times

#### Enable/Disable (Line ~1177-1213)
- `on_menu_par_state_selected()` - Enter par state menu
- `ToggleParState()` - Enable/disable par times

#### Manage Times (Line ~1213-1279)
- `on_menu_par_times_selected()` - Enter par times list
- `DisplayParInfo()` - Show current par time
- `ParDown()`, `ParUp()` - Navigate par list

#### Edit Individual Times (Line ~1279-1455)
- `EditPar()` - Enter/exit edit mode
- `LeftCursor()`, `RightCursor()` - Move edit cursor
- `LCDCursor()` - Display cursor position
- `AdjustParTime()` - Core adjustment logic
- `IncreaseTime()`, `DecreaseTime()` - Modify par time

### 🔊 SOUND OUTPUT (Line ~1457-1470)
**Purpose**: Generate audio feedback
- `BEEP()` - Main beep sound
- `ButtonTone()` - Button press feedback

### 🚀 SETUP FUNCTIONS (Line ~1472-1577)
**Purpose**: Initialize system components
- `EEPROMSetup()` - Load/save persistent settings
- `MenuSetup()` - Build menu structure
- `LCDSetup()` - Initialize LCD display

### 🎮 INPUT HANDLING (Line ~1579-1930)
**Purpose**: Process user input and trigger events
- 26 `Handle*()` functions - Button action handlers
- `ButtonListener()` - Main button processing (table-driven)
- `ShotListener()` - Listen for shots

### ⚡ SETUP AND LOOP (Line ~1932-1953)
**Purpose**: Arduino core functions
- `setup()` - System initialization
- `loop()` - Main program loop

---

## Finding Functions Quickly

### By Feature
- **Timer operations**: TIMER FUNCTIONS section
- **Reviewing shots**: SHOT REVIEW FUNCTIONS section
- **Changing settings**: SETTINGS FUNCTIONS section (5 subsections)
- **Par times**: PAR TIME FUNCTIONS section (3 subsections)
- **State changes**: STATE MANAGEMENT section
- **Button handling**: INPUT HANDLING section

### By Type
- **Menu callbacks**: Search for `on_menu_*_selected()`
- **State entry/exit**: Search for `Enter*State()` or `Exit*State()`
- **Increment/decrement**: Search for `Increase*()` or `Decrease*()`
- **Display helpers**: Look in section relevant to feature
- **Button handlers**: INPUT HANDLING > Button Handler Helper Functions

### Common Tasks

**Adding a new setting:**
1. Add constants in CONSTANTS section
2. Add menu item in MenuSetup()
3. Add `on_menu_*_selected()` in appropriate SETTINGS subsection
4. Add adjustment functions near menu callback
5. Add button handlers in INPUT HANDLING
6. Add EEPROM handling in EEPROMSetup() if needed

**Adding a new state:**
1. Add enum value in ProgramState
2. Add `Enter*State()` in STATE MANAGEMENT > State Entry Handlers
3. Add `Exit*State()` in STATE MANAGEMENT > State Exit Handlers
4. Add case in `TransitionToState()` switch statements
5. Add button mapping in `ButtonListener()`

**Modifying a feature:**
1. Locate the main section for that feature
2. All related functions should be nearby
3. Check INPUT HANDLING for button handlers
4. Check STATE MANAGEMENT for state entry/exit if applicable

---

## Section Header Format

```cpp
////////////////////////////////////////////////////////////
// MAJOR SECTION NAME
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// SUB-SECTION NAME (if applicable)
////////////////////////////////////////////////////////////

//////////////////////////////
// Function Description
//////////////////////////////

void FunctionName() {
  // Implementation
}
```

---

## Tips

1. **Use IDE's outline view** to see all section headers
2. **Search for section comments** to jump to areas quickly
3. **Related functions are adjacent** - scroll up/down to find helpers
4. **Consistent naming** - functions follow patterns within sections
5. **Menu callbacks** always named `on_menu_*_selected()`

