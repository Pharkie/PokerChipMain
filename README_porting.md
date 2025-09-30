# MicroPython → C++ Porting Checklist

This project converts the original UIFlow 2 (MicroPython + LVGL) PokerChip app to a C++ LVGL application running under PlatformIO on the M5Stack Dial.

## Build & Flash

```bash
pio run
pio run --target upload
pio run --target uploadfs  # if LittleFS assets are generated
```

## Workflow

1. Drop the MicroPython sources under `mp_source/`.
2. Run the conversion assistant to generate the `src/ui`, `src/input`, and `src/tasks` C++ modules.
3. If images or fonts are referenced, add raw files under `./assets/` and execute:
   ```bash
   python3 tools/pack_assets.py
   ```
   This copies the raw assets into `./data/` (LittleFS) and emits a stub `src/ui/assets_gen.cpp` ready for LVGL descriptors.
4. Build and flash as shown above.

## Runtime Contract

- `setup()` initialises M5Unified, LVGL, encoder input, and builds the widget tree via `ui::ui_init()`.
- `loop()` keeps LVGL responsive (`m5dial_lvgl_next()`) and lets the application logic poll for button clicks.
- Rotary events are delivered through the LVGL encoder driver; button clicks fall back to `M5.BtnA`.

## Test Plan

1. Confirm the PokerChip screen builds without LVGL assertions.
2. Rotate the dial to adjust the starting small blind (screen 1) and the minutes between rounds (screen 2).
3. Press the dial button to advance screens and start the active round timer.
4. Observe the countdown updating every second and the blinds doubling (with sound cues) when the timer elapses.
