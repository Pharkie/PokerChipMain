# PokerChipMain

PlatformIO + ESP-IDF project for the M5Stack Dial using the LVGL helper component.

## Requirements
- PlatformIO (VS Code extension or CLI)
- Espressif ESP-IDF toolchain provided by PlatformIO

## Versions
- PlatformIO platform: **espressif32@6.5.0**
- Bundled ESP-IDF: **5.1.2**
- LVGL: **9.x** (earlier versions are incompatible)

## Build, Flash, Monitor
From the project root run:

```bash
pio run
pio run --target upload
pio device monitor
```

You can also use the PlatformIO tasks in VS Code (Build, Upload, Monitor).

## Project Layout
- `platformio.ini` — PlatformIO environment configured for the M5Stack StampS3 board.
- `src/main.cpp` — application entry point that starts LVGL and renders the UI.
- `src/idf_component.yml` — declares the local LVGL helper component dependency.
- `components/m5dial_lvgl/` — LVGL integration layer shared with this project.


## Notes
- Keep the repository path free of spaces and outside synced-folders to avoid ESP-IDF build issues.
- Call `m5dial_lvgl_init()` before rendering LVGL widgets and run `m5dial_lvgl_next()` regularly to service the GUI.

## License
Project code is released under the [Creative Commons Attribution-NonCommercial 4.0 International](LICENSE) license.
Commercial use requires separate permission from the project authors.
Third-party libraries retain their upstream licenses (LVGL MIT, M5Unified MIT, ESP-IDF Apache-2.0, FreeRTOS MIT).
