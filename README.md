# PokerChip - M5Stack Dial Poker Timer

Poker timer for M5Stack Dial with rotary encoder control, touch interface, and automatic blind progression.

## Features

- Hardware-accelerated UI (LVGL 9.x on ESP-IDF)
- OOP screen architecture with reusable screen manager pattern
- Touch + rotary encoder dual input modes
- Hardware PCNT encoder (zero CPU overhead)
- Debounced button input (100ms software debounce, M5.BtnA doesn't work on Dial)
- Pause menu system (touch or encoder navigation)
- Automatic blind doubling with configurable round timer

## Quick Start

```bash
pio run                    # Build
pio run --target upload    # Flash to M5Stack Dial
pio device monitor         # Serial monitor (115200 baud)
```

Or use PlatformIO tasks in VS Code (Build, Upload, Monitor).

## For M5Stack Dial Developers

**[See docs/](docs/)** for reusable architecture patterns:

- Why M5Unified is kept despite limitations
- Hardware abstraction modules (Button, Encoder, Buzzer)
- What works and what doesn't on the Dial
- Quick start templates

## Project Structure

```
src/
├── hardware/        # Button, Encoder, Buzzer abstraction
├── screens/         # OOP screen architecture
├── ui/              # LVGL widgets and assets
└── main.cpp         # Entry point
```

See [docs/](docs/) for architecture patterns and development guides.

## Requirements

- PlatformIO (VS Code extension or CLI)
- ESP-IDF 5.1.2 (via PlatformIO espressif32@6.5.0)
- M5Stack Dial hardware

## Key Notes

- **M5.BtnA API doesn't work** - Use GPIO polling (see [docs/m5unified-architecture.md](docs/m5unified-architecture.md))
- **Keep repo outside cloud-synced folders** - Avoid Dropbox/iCloud (ESP-IDF build issues)
- **No spaces in path** - ESP-IDF requirement

## License

Project code is released under the [Creative Commons Attribution-NonCommercial 4.0 International](LICENSE) license.
Commercial use requires separate permission from the project authors.
Third-party libraries retain their upstream licenses (LVGL MIT, M5Unified MIT, ESP-IDF Apache-2.0, FreeRTOS MIT).
