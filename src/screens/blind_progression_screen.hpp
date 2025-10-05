#pragma once

#include "screen.hpp"

/// Screen for selecting blind progression rate (TURBO/STANDARD/RELAXED).
/// Displays choice name, description, and estimated game time.
class BlindProgressionScreen : public Screen {
public:
    /// Get the singleton instance.
    static BlindProgressionScreen& instance();

    // Screen interface implementation
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;

private:
    BlindProgressionScreen() = default;

    static constexpr int kOptionCount = 3;
    static constexpr float kMultipliers[] = {1.5f, 2.0f, 1.25f};
    static constexpr const char* kNames[] = {"STANDARD", "TURBO", "RELAXED"};
    static constexpr const char* kDescriptions[] = {
        "Blinds +50%/round",
        "Blinds double/round",
        "Blinds +25%/round"
    };

    // Starting stack: 16×25 + 20×50 + 6×100 = 2000 chips
    static constexpr int kStartingStack = 2000;

    // Sound feedback tones
    static constexpr float kToneUp = 3520.0f;      // A7
    static constexpr float kToneDown = 2793.0f;    // F7
    static constexpr float kToneBoundary = 1661.0f; // G#6
    static constexpr uint32_t kToneDuration = 120;

    int selection_ = 0;  // Default to STANDARD (now first item)
    char game_time_buffer_[32];  // Buffer for dynamic game time string

    void update_display();
    int calculate_estimated_rounds(float multiplier) const;

    // Touch handlers
    static void push_button_clicked_cb(lv_event_t* e);
    static void info_button_clicked_cb(lv_event_t* e);
    static void info_overlay_clicked_cb(lv_event_t* e);
    void show_info();
    void hide_info();
};
