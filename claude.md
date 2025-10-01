# PokerChip - M5Stack Dial Poker Timer

## Project Purpose
Poker chip timer application for the M5Stack Dial device. Converted from UIFlow 2 (MicroPython + LVGL) to native C++ using PlatformIO and ESP-IDF.

## Hardware Platform
- **Device**: M5Stack Dial (StampS3 board)
- **MCU**: ESP32-S3
- **Display**: 240x240 round LCD with LVGL 9.x
- **Input**: Rotary encoder + center button
- **Button A**: GPIO 42 (physical button on device)
- **Audio**: Built-in speaker for sound feedback

## Architecture Overview

### Core Systems
- **UI Framework**: LVGL 9.x via custom `m5dial_lvgl` component
- **RTOS**: FreeRTOS (via ESP-IDF)
- **Graphics**: Hardware-accelerated rendering through M5Unified library
- **Input**: Encoder mapped to LVGL input group system

### Code Structure
```
src/
├── main.cpp                          # Entry point: setup() and loop()
├── app_main_bridge.cpp              # ESP-IDF bridge
├── ui/
│   ├── ui_root.cpp                  # Main UI container and widget handles
│   ├── ui_root.hpp
│   ├── ui_assets.cpp                # Asset loading (images, fonts)
│   └── ui_assets.hpp
├── input/
│   └── encoder_input.cpp            # Rotary encoder → LVGL integration
├── tasks/
│   ├── app_tasks.cpp                # Main app coordination and button handling
│   ├── app_tasks.hpp
│   ├── small_blind_screen.cpp       # Starting blind configuration screen
│   └── small_blind_screen.hpp
└── images/
    ├── riccy.png                    # Boot splash image
    └── riccy_png.S                  # Embedded binary asset
```

## Current Implementation Status

### ✅ Completed Features
- **Boot Sequence**: Splash screen (riccy.png) with dual-tone startup sound (E7→E8, 120ms + 150ms)
- **Small Blind Screen**:
  - Rotary encoder adjusts value (25-200 in steps of 25)
  - Sound feedback: A7 (up), F7 (down), G#6 (boundary)
  - Large centered number display
  - Title: "Starting small blinds"
  - Push prompt at bottom
- **Button A Detection**: GPIO 42 polling in `app_tasks::tick()`
- **Encoder Input**: LVGL input group integration with key event callbacks

### ⚠️ In Progress / Incomplete
- Big blind configuration screen
- Timer countdown functionality
- Screen state transitions (button click handlers)
- Blind progression/doubling logic
- Round duration configuration
- Active game timer with elapsed time display

## Key Configuration Constants

### Small Blind Screen ([src/tasks/small_blind_screen.cpp:20-26](src/tasks/small_blind_screen.cpp#L20-L26))
```cpp
constexpr int kStep = 25;
constexpr int kMin = 25;
constexpr int kMax = 200;
constexpr float kToneUp = 3520.0f;      // A7
constexpr float kToneDown = 2793.0f;    // F7
constexpr float kToneBoundary = 1661.0f; // G#6
constexpr uint32_t kToneDuration = 120;  // milliseconds
```

### Hardware Pins ([src/tasks/app_tasks.cpp:19](src/tasks/app_tasks.cpp#L19))
```cpp
constexpr gpio_num_t kBtnAPin = GPIO_NUM_42;
```

## Development Workflow

### Build & Flash
```bash
pio run                    # Build firmware
pio run --target upload    # Flash to device
pio device monitor         # Serial monitor (115200 baud)
```

### PlatformIO Configuration
- **Platform**: espressif32@6.5.0
- **Framework**: espidf (v5.1.2)
- **Board**: m5stack-stamps3
- **Upload speed**: 1500000
- **Monitor filters**: esp32_exception_decoder

### Dependencies
- M5Unified: 0.1.12
- LVGL: 9.0.0
- Custom component: `m5dial_lvgl` (in `components/`)

## Main Program Flow

### setup() - [src/main.cpp:18-51](src/main.cpp#L18-L51)
1. Initialize M5Unified hardware
2. Display boot splash image
3. Play startup sound sequence
4. Initialize LVGL (`m5dial_lvgl_init()`)
5. Build UI tree (`ui::ui_init()`)
6. Setup encoder input
7. Initialize app tasks (show small blind screen)

### loop() - [src/main.cpp:53-59](src/main.cpp#L53-L59)
1. Update M5Unified (button states, etc.)
2. Service LVGL (`m5dial_lvgl_next()`)
3. Run app task polling (`app_tasks::tick()` - button detection)
4. 5ms delay

## UI Widget Handles
Available through `ui::get()` in [src/ui/ui_root.hpp](src/ui/ui_root.hpp):
- `logo` - Boot logo
- `page_title` - Screen title label
- `big_number` - Large centered value display
- `push_text` / `pushtext_bg` - Bottom prompt
- `down_arrow` - Visual indicator
- `small_blind_active` / `big_blind_active` - Active game displays
- `elapsed_mins` / `elapsed_secs` - Timer displays
- `focus_proxy` - LVGL input group target

## Input System
- **Encoder events** routed through LVGL input group → `encoder_key_event_cb()` in [src/tasks/app_tasks.cpp:21](src/tasks/app_tasks.cpp#L21)
- **Button A** polled directly via GPIO in `app_tasks::tick()` (press/release/click detection)
- Encoder delta mapped to: `LV_KEY_RIGHT/NEXT` (CW) and `LV_KEY_LEFT/PREV` (CCW)

## Next Development Steps

### Priority Tasks
1. **Screen State Machine**: Implement transitions between configuration screens and active game
2. **Big Blind Screen**: Clone small blind screen with appropriate range/title
3. **Round Duration Screen**: Configure minutes per round
4. **Active Game Timer**: Countdown display with 1-second tick updates
5. **Blind Doubling Logic**: Auto-advance when timer expires
6. **Sound Cues**: Timer warnings, round transitions

### Button A Handler
Currently logs press/release/click events. Needs implementation:
- Advance from small blind → big blind → duration → start game
- Pause/resume timer during active game
- Return to configuration from game

## Notes
- Repository must be kept outside cloud-synced folders (Dropbox, iCloud) to avoid ESP-IDF build issues
- Avoid spaces in project path
- LVGL 9.x required (earlier versions incompatible)
- Custom `m5dial_lvgl` component handles display driver and touch/encoder integration
- Git branch: `main` (no separate main branch configured)

## License
Creative Commons Attribution-NonCommercial 4.0 International
Commercial use requires separate permission from project authors.
