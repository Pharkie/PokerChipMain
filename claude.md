# PokerChip - M5Stack Dial Poker Timer

## Project Purpose
Poker chip timer application for the M5Stack Dial device. Converted from UIFlow 2 (MicroPython + LVGL) to native C++ using PlatformIO and ESP-IDF.

## Hardware Platform
- **Device**: M5Stack Dial (StampS3 board)
- **MCU**: ESP32-S3
- **Display**: 240x240 round LCD with LVGL 9.x
- **Input**: Rotary encoder + center button
- **Button A**: GPIO 42 (physical button on device)
  - **⚠️ IMPORTANT**: M5Unified's `M5.BtnA` Button_Class does NOT work with this device
  - **Must use direct GPIO polling** via ESP-IDF `gpio_get_level()` API
- **Audio**: Built-in speaker for sound feedback

## Architecture Overview

### Core Systems
- **UI Framework**: LVGL 9.x via custom `m5dial_lvgl` component
- **RTOS**: FreeRTOS (via ESP-IDF)
- **Graphics**: Hardware-accelerated rendering through M5Unified library
- **Input**: Encoder mapped to LVGL input group system

## M5Unified & Hardware Abstraction

### M5Unified Role
M5Unified is kept as a core dependency because it provides:
- **Display Driver**: M5GFX library for GC9A01 LCD controller (via `M5.Display.*`)
- **Touch Driver**: FT3267 touchscreen I2C driver (via `M5.Touch.*`)
- **Speaker/Buzzer**: LEDC PWM tone generation on GPIO3 (via `M5.Speaker.tone()`)
- **Power Control**: GPIO46 control for power-off (via `M5.Power.powerOff()`)

These components work reliably and are well-tested across the M5Stack ecosystem.

### Why We Keep M5Unified
**Decision**: Use M5Unified where it works, extend with custom modules where needed.

**Rationale**:
1. Display and touch drivers are mature, tested, and work perfectly
2. The `m5dial_lvgl` component already depends on M5Unified for display/touch integration
3. M5GFX is bundled with M5Unified (not worth extracting separately)
4. Speaker API (`M5.Speaker.tone()`) provides clean PWM tone generation
5. Removing M5Unified would require rewriting ~500+ lines of display/touch driver code with high risk of bugs

### Known M5Unified Limitations on M5Stack Dial
- **Button_Class API does NOT work**: `M5.BtnA.wasClicked()`, `M5.BtnA.isPressed()`, etc. are non-functional
  - This is a known hardware/library incompatibility with the StampS3-based Dial device
  - **Must use direct GPIO polling** via ESP-IDF `gpio_get_level(GPIO_NUM_42)`
- **Encoder API not used**: M5Unified's encoder support is less efficient than direct hardware PCNT

### Hardware Abstraction Layer
To work around M5Unified limitations and provide cleaner APIs, we built custom hardware modules:

#### [src/hardware/config.hpp](src/hardware/config.hpp)
**Purpose**: Centralized hardware configuration constants
- Single source of truth for GPIO pin assignments
- Timing constants (debounce, long press thresholds)
- Easy to port to different hardware variants

**Example**:
```cpp
namespace hardware::config {
    namespace pins {
        constexpr gpio_num_t BUTTON_A = GPIO_NUM_42;
    }
    namespace button {
        constexpr uint32_t DEBOUNCE_MS = 100;
        constexpr uint32_t LONG_PRESS_MS = 2000;
    }
}
```

#### [src/hardware/button.hpp](src/hardware/button.hpp) / [.cpp](src/hardware/button.cpp)
**Purpose**: Debounced button input with short/long press detection
- Replaces broken `M5.BtnA` Button_Class API
- Uses direct GPIO polling with configurable debounce
- Callback-based API: `on_short_press()`, `on_long_press()`
- Static allocation (zero heap usage)
- Fully reusable for any GPIO button

**Usage Example**:
```cpp
// Static allocation using config constants
static hardware::Button btnA(
    hardware::config::pins::BUTTON_A,
    hardware::config::button::DEBOUNCE_MS,
    hardware::config::button::LONG_PRESS_MS
);
btnA.on_short_press([]() { /* handle click */ });
btnA.on_long_press([]() { M5.Power.powerOff(); });
btnA.update();  // Call from main loop
```

#### [src/hardware/encoder.hpp](src/hardware/encoder.hpp) / [.cpp](src/hardware/encoder.cpp)
**Purpose**: Rotary encoder input via hardware PCNT peripheral
- Uses ESP32-S3's hardware pulse counter (more efficient than M5Unified's implementation)
- Bridges low-level C callback (`encoder_notify_diff`) to C++ OOP
- Callback-based API: `on_rotation(delta)`
- Zero CPU overhead (hardware counting with glitch filter)

**Usage Example**:
```cpp
hardware::Encoder::instance().on_rotation([](int delta) {
    ScreenManager::instance().handle_encoder(delta);
});
```

### Architecture Benefits
✅ **Pragmatic**: Use M5Unified where it works well (display, touch, speaker)
✅ **Clean APIs**: Hardware abstraction modules provide consistent callback-based interfaces
✅ **Testable**: Hardware modules can be mocked for unit testing
✅ **Maintainable**: Clear separation between M5Unified usage and custom extensions
✅ **Efficient**: Hardware PCNT encoder, debounced button input with minimal CPU usage

### Code Structure
```
src/
├── main.cpp                          # Entry point: setup() and loop()
├── app_main_bridge.cpp              # ESP-IDF bridge
├── hardware/                         # Hardware abstraction modules
│   ├── config.hpp                    # Hardware constants (pins, timings)
│   ├── button.hpp/cpp                # Debounced GPIO button (replaces M5.BtnA)
│   └── encoder.hpp/cpp               # PCNT encoder wrapper
├── screens/                          # OOP screen architecture
│   ├── screen.hpp/cpp                # Abstract base class
│   ├── screen_manager.hpp/cpp        # Singleton screen dispatcher
│   ├── small_blind_screen.hpp/cpp    # Config screen 1
│   ├── round_minutes_screen.hpp/cpp  # Config screen 2
│   ├── blind_progression_screen.hpp/cpp  # Config screen 3
│   └── game_active_screen.hpp/cpp    # Active game timer
├── ui/
│   ├── ui_root.cpp                   # Widget initialization and groups
│   ├── ui_root.hpp                   # Widget handles and group management
│   ├── ui_assets.cpp                 # Asset loading (images, fonts)
│   └── ui_assets.hpp
├── input/
│   └── encoder_input.cpp             # Rotary encoder → LVGL integration
├── game_state.hpp/cpp                # Encapsulated game state singleton
└── images/
    ├── riccy.png                     # Boot splash image
    └── riccy_png.S                   # Embedded binary asset
```

## Current Implementation Status

### ✅ Completed Features
- **Boot Sequence**: Splash screen (riccy.png) with dual-tone startup sound (E7→E8, 120ms + 150ms)
- **Hardware Abstraction Layer**:
  - Button module with 100ms debouncing and short/long press detection
  - Encoder module using hardware PCNT peripheral
  - Speaker/buzzer tones via M5.Speaker.tone() directly
- **OOP Screen Architecture**:
  - Abstract Screen base class with lifecycle hooks and modal overlay support
  - ScreenManager singleton for screen transitions
  - GameState singleton with encapsulated fields and validation
  - Widget group system for declarative UI management
- **Small Blind Screen**:
  - Rotary encoder adjusts value (25-200 in steps of 25)
  - Sound feedback: A7 (up), F7 (down), G#6 (boundary)
  - Large 48pt centered number display
  - Transitions to Round Minutes screen on button press
- **Round Minutes Screen**:
  - Rotary encoder adjusts value (5-45 minutes in steps of 5)
  - Same UI pattern as Small Blind screen
  - Transitions to Active Game screen on button press
- **Active Game Timer**:
  - Countdown timer with 1-second tick updates
  - Round number, small blind, big blind display (48pt font)
  - Automatic blind doubling when timer expires
  - Musical tone sequence on round transitions
  - Pause menu system with touch and rotary+button controls
- **Pause Menu System**:
  - Tap menu button or press Button A to pause
  - 5 menu items: Resume, Reset, Skip Round, Settings (placeholder), Power Off
  - Navigate with rotary encoder or tap menu items directly
  - Resume/skip round functionality, reset to config screen
- **Button A**:
  - 100ms debounced GPIO polling (replaces broken M5.BtnA API)
  - Short press (<2s) opens pause menu
  - Long press (≥2s) powers off device
- **Display Layout**:
  - Optimized for 240x240 circular display
  - Menu button centered at bottom for visibility
  - Proper spacing for 48pt fonts
- **Info Overlay System**:
  - Tap "i" button to view upcoming blind schedule
  - Modal overlay blocks underlying screen interactions
  - Close button positioned at (130, 20) for visibility

### ⚠️ Known Limitations
- Settings menu item is placeholder (not implemented)
- No persistent storage (configuration resets on power cycle)

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

### Hardware Configuration ([src/hardware/config.hpp](src/hardware/config.hpp))
All hardware constants centralized:
```cpp
namespace hardware::config::pins {
    constexpr gpio_num_t BUTTON_A = GPIO_NUM_42;
}
namespace hardware::config::button {
    constexpr uint32_t DEBOUNCE_MS = 100;
    constexpr uint32_t LONG_PRESS_MS = 2000;
}
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

## UI System

### Widget Handles
Available through `ui::get()` in [src/ui/ui_root.hpp](src/ui/ui_root.hpp):
- `logo`, `page_title`, `big_number` - Core display elements
- `push_text`, `pushtext_bg`, `down_arrow` - Bottom prompt area
- `small_blind_active`, `big_blind_active` - Active game displays
- `elapsed_mins`, `elapsed_secs` - Timer displays
- `menu_button`, `menu_overlay`, `menu_item_*` - Pause menu system
- `info_button`, `info_overlay`, `info_close_button` - Info overlay system
- `focus_proxy` - LVGL input group target

### Widget Groups
Declarative UI management via `ui::groups()`:
- `config_common` - Shared widgets for configuration screens
- `game_active` - Active game timer screen widgets
- `menu` / `info` - Overlay widgets (managed by show_menu/hide_menu, show_info/hide_info)

Helper functions:
```cpp
ui::show_group(ui::groups().config_common);  // Show all widgets in group
ui::hide_all_groups();  // Hide all managed groups
```

## Input System

### Hardware Modules (see "M5Unified & Hardware Abstraction" section above)
- **Button A** - [`hardware::Button`](src/hardware/button.hpp) instance on GPIO 42
  - 100ms debounce, short press (<2s) and long press (≥2s) detection
  - Replaces broken `M5.BtnA` API with direct GPIO polling
  - Callbacks route to ScreenManager for short press, M5.Power.powerOff() for long press
- **Rotary Encoder** - [`hardware::Encoder`](src/hardware/encoder.hpp) singleton
  - Hardware PCNT peripheral (zero CPU overhead, 1000ns glitch filter)
  - Low-level C callback `encoder_notify_diff()` bridges to C++ singleton
  - Rotation callback routes to ScreenManager which forwards to active screen
- **Touch** - Via M5Unified's touch driver in `m5dial_lvgl` component
  - FT3267 touchscreen I2C driver
  - LVGL pointer input device type
  - Used for menu button and pause menu item taps

### Input Flow
1. **Encoder rotation** → PCNT hardware counts → `encoder_notify_diff(int diff)` → `hardware::Encoder::notify_rotation()` → registered callback → `ScreenManager::handle_encoder()` → active screen's `handle_encoder()`
2. **Button A press** → GPIO polling in `hardware::Button::update()` → debounce logic → short/long press detection → registered callbacks
3. **Touch events** → M5.Touch I2C driver → LVGL pointer indev → LVGL event callbacks (e.g., `LV_EVENT_CLICKED` on menu button/items)

## Architecture Design Principles

### Memory Management
- **Static allocation preferred** - No heap allocations in main loop or hardware modules
- **Widget groups** - Declarative UI management, reduces code duplication
- **Global widget pool** - More memory-efficient than LVGL screen objects per app screen

### State Management
- **Single source of truth** - Use LVGL widget visibility instead of duplicate boolean flags
- **Encapsulation** - GameState uses private fields with validated setters
- **Modal overlays** - `is_modal_blocking()` helper prevents underlying screen input

### Hardware Abstraction
- **Keep M5Unified** for display, touch, speaker, power (where it works)
- **Extend with custom modules** for button (GPIO polling) and encoder (PCNT)
- **Centralized config** - All hardware constants in `hardware/config.hpp`

## Notes
- Repository must be kept outside cloud-synced folders (Dropbox, iCloud) to avoid ESP-IDF build issues
- Avoid spaces in project path
- LVGL 9.x required (earlier versions incompatible)
- Custom `m5dial_lvgl` component handles display driver and touch/encoder integration
- Git branch: `main` (no separate main branch configured)

## License
Creative Commons Attribution-NonCommercial 4.0 International
Commercial use requires separate permission from project authors.
