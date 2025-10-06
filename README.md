# PokerChip - M5Stack Dial Poker Timer

Turn your M5Stack Dial into the heartbeat of your poker night. Spin the dial, set your game, and let PokerChip take over — managing blinds, timing each round, and bebooping to keep you on track while you focus on the cards.

![PokerChip Demo](docs/assets/PokerOct25.gif)

## Features

- **Configurable blinds** - Set starting small blind (25-200)
- **Round timer** - Choose round length (5-45 minutes)
- **Blind progression modes** - Standard (1.5x), Turbo (2x), or Relaxed (1.25x)
- **Pause menu** - Resume, skip rounds, adjust volume, start new game
- **Chip breakdown overlay** - Quick reference for starting stacks
- **Audio feedback** - Musical tones for all interactions

## Quick Start

Use PlatformIO IDE in VS Code.

```bash
pio run --target upload    # Flash to M5Stack Dial
```

If anyone wants a prebuilt firmware, let me know.

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

## License

Project code is released under the [Creative Commons Attribution-NonCommercial 4.0 International](LICENSE) license.
Commercial use requires separate permission from the project authors.
Third-party libraries retain their upstream licenses (LVGL MIT, M5Unified MIT, ESP-IDF Apache-2.0, FreeRTOS MIT).
