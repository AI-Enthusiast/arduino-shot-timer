## Key Cleanup Opportunities:

1. **✅ COMPLETED - Reduce code duplication** - Many functions like `NextShot()`/`PreviousShot()`, `IncreaseDelay()`/`DecreaseDelay()`, etc. had nearly identical code
2. **✅ COMPLETED - Refactor large switch statements** - The `ButtonListener()` function had a massive nested switch statement (~300 lines) that has been refactored into a clean table-driven design using function pointers. See `cleanup/2/` for details.
3. **✅ COMPLETED - Extract magic numbers** - All hardcoded values (625, 650, 255, timing constants, display widths, etc.) have been extracted into well-organized named constants. See `cleanup/3/` for details.
4. **✅ COMPLETED - Simplify state management** - Implemented centralized state machine pattern with entry/exit handlers and state utility functions. See `cleanup/4/` for details.
5. **✅ COMPLETED - Consolidate LCD operations** - Consolidated all repeated LCD operation patterns by adding 10 new helper functions to `LCDHelpers.h` and refactoring 19 functions throughout the codebase. Reduced ~60 lines of duplicated code and improved readability significantly. See `cleanup/5/` for details.
6. **✅ COMPLETED - Remove commented/dead code** - Removed all TODO comments, commented-out DEBUG lines, old dead code blocks, and improved function headers with descriptive comments. Removed 18 lines of clutter while preserving all active DEBUG macros. See `cleanup/6/` for details.
7. **✅ COMPLETED - Improve function organization** - Reorganized all functions into clear, logical sections with hierarchical structure. Added 10 major section headers grouping functions by feature area (State Management, Timer, Review, Settings, Par Times, Sound, Setup, Input Handling). Significantly improved code navigation and maintainability. See `cleanup/7/` for details.
8. **✅ COMPLETED - Reduce global variables** - Many globals could be encapsulated in structs/classes
9. **Simplify boundary checking** - Functions like `IncreaseTime()`/`DecreaseTime()` have repetitive boundary checks
10. **Better naming** - Some function names could be more descriptive (e.g., `BEEP()` vs `PlayBeepSound()`)
