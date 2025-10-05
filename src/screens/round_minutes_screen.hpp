#pragma once

#include "screen.hpp"

/// Configuration screen for setting minutes between rounds.
/// Allows selection from 5-45 minutes in steps of 5 using rotary encoder.
/// Plays sound feedback for value changes and boundary hits.
class RoundMinutesScreen : public Screen {
public:
    /// Get the singleton instance.
    static RoundMinutesScreen& instance();

    // Screen interface implementation
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;

private:
    RoundMinutesScreen() = default;

    int value_ = 15;

    static constexpr int kStep = 5;
    static constexpr int kMin = 5;
    static constexpr int kMax = 45;
    static constexpr float kToneUp = 2637.0f;        // E7 (high = increment)
    static constexpr float kToneDown = 1760.0f;      // A6 (low = decrement)
    static constexpr float kToneBoundary = 1245.0f;  // D#6 (lower = blocked)
    static constexpr uint32_t kToneDuration = 60;    // milliseconds (quick feedback)

    void update_display();

    // Touch handlers
    static void push_button_clicked_cb(lv_event_t* e);
    static void info_button_clicked_cb(lv_event_t* e);
    static void info_overlay_clicked_cb(lv_event_t* e);
    void show_info();
    void hide_info();
};
