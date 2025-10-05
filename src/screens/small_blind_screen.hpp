#pragma once

#include "screen.hpp"

/// Configuration screen for setting the starting small blind value.
/// Allows selection from 25-200 in steps of 25 using rotary encoder.
/// Plays sound feedback for value changes and boundary hits.
class SmallBlindScreen : public Screen {
public:
    /// Get the singleton instance.
    static SmallBlindScreen& instance();

    // Screen interface implementation
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;

private:
    SmallBlindScreen() = default;

    int value_ = 25;

    static constexpr int kStep = 25;
    static constexpr int kMin = 25;
    static constexpr int kMax = 200;
    static constexpr float kToneUp = 4186.0f;        // C8 (high = increment)
    static constexpr float kToneDown = 2093.0f;      // C7 (low = decrement)
    static constexpr float kToneBoundary = 1568.0f;  // G6 (lower = blocked)
    static constexpr uint32_t kToneDuration = 60;    // milliseconds (quick feedback)

    void update_display();

    // Touch handlers
    static void push_button_clicked_cb(lv_event_t* e);
    static void info_button_clicked_cb(lv_event_t* e);
    static void info_overlay_clicked_cb(lv_event_t* e);
    void show_info();
    void hide_info();
};
