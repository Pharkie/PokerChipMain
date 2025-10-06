#pragma once

#include <lvgl.h>
#include <cstdint>
#include "ui/ui_root.hpp"

/// Abstract base class for all application screens.
/// Provides lifecycle hooks, input handling, and common utilities.
class Screen {
public:
    virtual ~Screen() = default;

    /// Create this screen's LVGL widgets.
    /// Called before on_enter() during screen transition.
    virtual void create_widgets() = 0;

    /// Destroy this screen's LVGL widgets.
    /// Called after on_exit() during screen transition.
    virtual void destroy_widgets() = 0;

    /// Called when this screen becomes active.
    /// Use this to setup UI widgets and initialize state.
    virtual void on_enter() = 0;

    /// Called when this screen is deactivated.
    /// Use this to cleanup resources (optional - default is no-op).
    virtual void on_exit() {}

    /// Called when this screen is temporarily backgrounded (future use).
    virtual void on_suspend() {}

    /// Called when this screen returns from background (future use).
    virtual void on_resume() {}

    /// Handle rotary encoder rotation.
    /// @param diff Raw encoder delta value (positive = CW, negative = CCW)
    virtual void handle_encoder(int diff) = 0;

    /// Handle button A click (press + release).
    virtual void handle_button_click() = 0;

    /// Called every frame from main loop (optional).
    /// Use for animations, timers, or continuous updates.
    /// Default implementation does nothing.
    virtual void tick() {}

protected:
    /// Get access to shared LVGL UI widget handles.
    const ui::Handles& ui() const;

    /// Helper to show/hide LVGL objects.
    void set_visible(lv_obj_t* obj, bool visible);

    /// Helper to play a tone through M5 speaker.
    void play_tone(float freq_hz, uint32_t duration_ms);

    /// Check if a modal overlay is currently blocking input.
    /// Uses LVGL widget visibility as single source of truth.
    /// Default implementation returns false (no modals).
    virtual bool is_modal_blocking() const { return false; }
};
