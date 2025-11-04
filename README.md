# PokerChip - M5Stack Dial Poker Timer

Turn your M5Stack Dial into the heartbeat of your poker night. Spin the dial, set your game, and let PokerChip take over — managing blinds, timing each round, and bebooping to keep you on track while you focus on the cards.

![PokerChip Demo](docs/assets/PokerOct25.gif)

## Features

### Poker Timer
- **Configurable blinds** - Set starting small blind (25-200 in steps of 25)
- **Round timer** - Choose round length (5-45 minutes in steps of 5)
- **Blind progression modes** - Standard (1.5x), Turbo (2x), or Relaxed (1.25x) with time estimates
- **Live countdown timer** - MM:SS display with automatic blind increases
- **Chip breakdown overlay** - Quick reference sheet for starting chip distribution

### Game Management
- **Pause menu** - Resume, skip round, adjust volume, view game logs, start new game, power off
- **Game logging system** - Stores last 50 games with stats (in-game time, paused time, max round reached)
- **Game history viewer** - Browse past games with page-based navigation
- **Volume control** - Adjustable speaker volume (0-10) persisted across reboots via NVS

### Hardware Integration
- **Rotary encoder** - Smooth, rotary, zero-lag navigation
- **Touch screen** - Screen tap support for buttons and menu items
- **Button A** - Short press for pause, long press (2s) for power-off
- **Audio feedback** - Musical tones for all interactions and round transitions
- **Boot splash** - Custom startup screen with musical greeting

## Quick Start

### Requirements
- M5Stack Dial device (StampS3 board with ESP32-S3)
- PlatformIO IDE in VS Code
- ESP-IDF 5.1.2 (auto-installed by PlatformIO)

### Build and Flash
```bash
pio run                    # Build firmware
pio run --target upload    # Flash to M5Stack Dial
pio device monitor         # View serial output (115200 baud)
```

If anyone wants a prebuilt firmware, let me know.

## Controls

- **Rotary dial** - Adjust values / navigate menus
- **Touch screen** - Tap buttons and menu items
- **Button A** (bottom button) - Confirm / close overlays
- **Long press Button A** (2s) - Power off

## Technical Highlights

### Architecture
- **2,695 lines of C++** with clean OOP design
- **7 distinct screens** with lifecycle management (create, enter, exit, destroy, tick)
- **Hardware abstraction layer** - Custom Button and Encoder modules that work around M5Unified limitations
- **LVGL 9.x UI framework** - Declarative widget group management
- **NVS persistent storage** - Volume settings and game logs survive power cycles

### Code Organization
```
src/
├── main.cpp                          # Entry point
├── hardware/                         # Hardware abstraction modules
│   ├── config.hpp                    # Centralized pin/timing constants
│   ├── button.hpp/cpp                # Debounced GPIO button (replaces broken M5.BtnA)
│   └── encoder.hpp/cpp               # PCNT hardware encoder wrapper
├── screens/                          # 7 screen implementations
│   ├── screen.hpp/cpp                # Abstract base class
│   ├── screen_manager.hpp/cpp        # Singleton screen dispatcher
│   └── [small_blind, round_minutes, blind_progression,
        game_active, volume, game_logs]_screen.hpp/cpp
├── storage/                          # Persistent storage
│   ├── nvs_storage.hpp/cpp           # Volume persistence
│   └── game_log.hpp/cpp              # 50-game ring buffer
├── ui/                               # LVGL UI system
│   ├── ui_root.cpp/hpp               # Widget pool and groups
│   ├── ui_helpers.hpp                # Prevents focus outline bugs
│   └── ui_styles.hpp/cpp             # Reusable LVGL styles
└── game_state.hpp/cpp                # Encapsulated singleton state
```

## For Developers

Building a project for M5Stack Dial? See [docs/](docs/) for reusable patterns:

### What Works with M5Unified on Dial
- Display driver (M5GFX for GC9A01 LCD)
- Touch driver (FT3267 touchscreen I2C)
- Speaker/buzzer (LEDC PWM tone generation)
- Power control (GPIO46 power-off)

### What Doesn't Work (and solutions)
- **Button_Class API is broken** → Custom [hardware/button.hpp](src/hardware/button.hpp) with direct GPIO polling
- **Encoder API is inefficient** → Custom [hardware/encoder.hpp](src/hardware/encoder.hpp) using ESP32-S3 PCNT peripheral
- **LVGL focus system conflicts with rotary input** → Custom [ui/ui_helpers.hpp](src/ui/ui_helpers.hpp) to disable click-focus on buttons

### Key Patterns
- Static allocation (zero heap usage in hot paths)
- Callback-based hardware abstraction
- Widget group system for declarative UI management
- Single source of truth for widget positioning

## License

Project code is released under the [Creative Commons Attribution-NonCommercial 4.0 International](LICENSE) license.
Commercial use requires separate permission from the project authors.
Third-party libraries retain their upstream licenses (LVGL MIT, M5Unified MIT, ESP-IDF Apache-2.0, FreeRTOS MIT).
