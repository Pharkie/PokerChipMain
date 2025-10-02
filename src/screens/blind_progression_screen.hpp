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
        "50% increase/round",
        "Doubles each round",
        "25% increase/round"
    };
    static constexpr const char* kGameTimes[] = {
        "Game time: ~60-75 mins",
        "Game time: ~45-60 mins",
        "Game time: ~90-120 mins"
    };

    // Sound feedback tones
    static constexpr float kToneUp = 3520.0f;      // A7
    static constexpr float kToneDown = 2793.0f;    // F7
    static constexpr float kToneBoundary = 1661.0f; // G#6
    static constexpr uint32_t kToneDuration = 120;

    int selection_ = 0;  // Default to STANDARD (now first item)

    void update_display();
};
