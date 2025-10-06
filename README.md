# PokerChip - M5Stack Dial Poker Timer

A poker timer for the M5Stack Dial. Set up your game with the dial, then let it manage blind levels automatically.

![PokerChip Demo](docs/assets/PokerOct25.gif)

## Features

- **Configurable blinds** - Set starting small blind (25-200)
- **Round timer** - Choose round length (5-45 minutes)
- **Blind progression modes** - Standard (1.5x), Turbo (2x), or Relaxed (1.25x)
- **Pause menu** - Resume, skip rounds, adjust volume, start new game
- **Chip breakdown overlay** - Quick reference for starting stacks
- **Audio feedback** - Musical tones for all interactions

## Quick Start

```bash
pio run --target upload    # Flash to M5Stack Dial
```

Or use PlatformIO IDE in VS Code.

## Controls

- **Rotary dial** - Adjust values / navigate menus
- **Touch screen** - Tap buttons and menu items
- **Button A** (bottom button) - Confirm / close overlays
- **Long press Button A** (2s) - Power off

## For Developers

Building a project for M5Stack Dial? See [docs/](docs/) for reusable patterns:
- Hardware abstraction modules (Button, Encoder)
- OOP screen architecture
- What works (and doesn't) with M5Unified on Dial

## Requirements

- PlatformIO
- M5Stack Dial hardware
- Keep repo outside cloud-synced folders (Dropbox/iCloud)

## License

Project code is released under the [Creative Commons Attribution-NonCommercial 4.0 International](LICENSE) license.
Commercial use requires separate permission from the project authors.
Third-party libraries retain their upstream licenses (LVGL MIT, M5Unified MIT, ESP-IDF Apache-2.0, FreeRTOS MIT).
