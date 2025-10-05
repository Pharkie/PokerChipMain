#pragma once

#include "screen.hpp"

/// Volume control screen - adjusts speaker volume from 0-10.
/// Uses rotary encoder for adjustment and displays large number.
/// Button A confirms and saves to NVS.
class VolumeScreen : public Screen {
public:
    /// Get the singleton instance.
    static VolumeScreen& instance();

    // Screen interface implementation
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;

private:
    VolumeScreen() = default;

    int value_ = 5;  // 0-10 scale

    static constexpr int kStep = 1;
    static constexpr int kMin = 0;
    static constexpr int kMax = 10;
    static constexpr float kToneUp = 4186.0f;        // C8 (high = increment)
    static constexpr float kToneDown = 2093.0f;      // C7 (low = decrement)
    static constexpr float kToneBoundary = 1568.0f;  // G6 (lower = blocked)
    static constexpr uint32_t kToneDuration = 60;    // milliseconds (quick feedback)

    void update_display();
    void apply_volume();  // Apply current volume to M5.Speaker

    // Touch handler
    static void push_button_clicked_cb(lv_event_t* e);
};
