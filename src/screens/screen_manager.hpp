#pragma once

#include "screen.hpp"

/// Manages the active screen and routes input events.
/// Implements singleton pattern for global access.
class ScreenManager {
public:
    /// Get the singleton instance.
    static ScreenManager& instance();

    /// Initialize the screen manager (call once at startup).
    void init();

    /// Transition to a new screen.
    /// Calls on_exit() on current screen, then on_enter() on next screen.
    /// @param next_screen Pointer to the new screen (must remain valid)
    void transition_to(Screen* next_screen);

    /// Get the currently active screen (may be nullptr).
    Screen* current() const { return current_; }

    /// Route encoder input to the active screen.
    void handle_encoder(int diff);

    /// Route button click to the active screen.
    void handle_button_click();

    /// Update active screen (called from main loop).
    void tick();

private:
    ScreenManager() = default;
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    Screen* current_ = nullptr;
};
