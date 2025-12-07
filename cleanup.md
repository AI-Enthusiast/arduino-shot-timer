Looking at this Arduino shot timer code, here are several ways I could help clean it up:

## Key Cleanup Opportunities:

1. **Reduce code duplication** - Many functions like `NextShot()`/`PreviousShot()`, `IncreaseDelay()`/`DecreaseDelay()`, etc. have nearly identical code
2. **Refactor large switch statements** - The `ButtonListener()` function has a massive nested switch statement (lines 1442-1682) that could be simplified
3. **Extract magic numbers** - Many hardcoded values like 625, 650, 255 should be constants
4. **Simplify state management** - Create helper functions or use a state machine pattern
5. **Consolidate LCD operations** - Many repeated LCD print patterns
6. **Remove commented/dead code** - Several TODO comments and debug blocks
7. **Improve function organization** - Group related functions better
8. **Reduce global variables** - Many globals could be encapsulated in structs/classes
9. **Simplify boundary checking** - Functions like `IncreaseTime()`/`DecreaseTime()` have repetitive boundary checks
10. **Better naming** - Some function names could be more descriptive (e.g., `BEEP()` vs `PlayBeepSound()`)

Would you like me to:
- **Create a pull request** with specific refactoring improvements?
- Focus on a **particular area** (like the button handling, LCD display code, or state management)?
- Show you **specific examples** of how certain sections could be improved?

What would be most helpful for you?