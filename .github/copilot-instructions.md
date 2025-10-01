# AI Agent Instructions for PokerChipMain

## Project Overview
This is an ESP32-based application for the M5Stack Dial device using LVGL for UI. The project implements a poker chip timer/counter interface with a rotary encoder input.

## Key Architecture Components
- **UI Layer** (`src/ui/`): LVGL-based interface components managed through `ui_root.hpp`
- **Input Handling** (`src/input/`): Encoder input processing via `encoder_input.hpp`
- **Task Management** (`src/tasks/`): Application tasks and state management in `app_tasks.hpp`
- **LVGL Integration** (`components/m5dial_lvgl/`): Custom LVGL port for M5Stack Dial

## Development Workflow
1. **Build & Deploy**:
   ```bash
   pio run                  # Build
   pio run --target upload  # Flash to device
   pio device monitor      # View serial output
   ```

2. **UI Development Pattern**:
   - UI elements are defined in `ui_root.hpp`
   - LVGL must be initialized via `m5dial_lvgl_init()` before UI creation
   - Always call `m5dial_lvgl_next()` in the main loop

3. **Input Handling**:
   - Encoder events are processed through `encoder_input::init()`
   - UI focus management uses `ui::get().focus_proxy`

## Project-Specific Conventions
1. **Component Dependencies**:
   - Local components are declared in `src/idf_component.yml`
   - External libs (LVGL, M5Unified) are managed via PlatformIO

2. **Image Assets**:
   - Store images in `src/images/`
   - Embedded as binary data (see riccy.png usage in `main.cpp`)

3. **Task Structure**:
   - Main application logic lives in `app_tasks` namespace
   - Tasks are initialized in `init()` and updated via `tick()`

## Critical Constraints
- ESP-IDF version: 5.1.2 (via platform espressif32@6.5.0)
- LVGL version: 9.x required
- Repository path must be free of spaces and outside synced folders
- Non-commercial license - reference LICENSE for usage terms

## Common Patterns
- UI updates are managed through the `ui::Handles` struct
- Task state changes flow through `app_tasks::tick()`
- Hardware access via M5Unified library (`M5.begin()`, `M5.update()`)