#pragma once

#include "screen.hpp"

/// Screen for selecting blind progression rate (TURBO/STANDARD/RELAXED).
/// Displays choice name, description, and estimated game time.
class BlindProgressionScreen : public Screen {
public:
    /// Get the singleton instance.
    static BlindProgressionScreen& instance();

    // Screen interface implementation
    void create_widgets() override;
    void destroy_widgets() override;
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;
    bool is_modal_blocking() const override;

private:
    BlindProgressionScreen() = default;

    // Widget handles
    lv_obj_t* title_bg_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* mode_name_ = nullptr;
    lv_obj_t* mode_description_ = nullptr;
    lv_obj_t* mode_game_time_ = nullptr;
    lv_obj_t* bottom_button_ = nullptr;
    lv_obj_t* confirm_label_ = nullptr;
    lv_obj_t* info_button_ = nullptr;
    lv_obj_t* info_overlay_ = nullptr;
    lv_obj_t* info_title_bg_ = nullptr;
    lv_obj_t* info_title_ = nullptr;
    lv_obj_t* info_blue_ = nullptr;
    lv_obj_t* info_white_ = nullptr;
    lv_obj_t* info_red_ = nullptr;
    lv_obj_t* info_stack_ = nullptr;
    lv_obj_t* info_close_button_ = nullptr;
    lv_obj_t* info_close_label_ = nullptr;

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
    static constexpr float kToneUp = 2637.0f;      // E7 (high = increment)
    static constexpr float kToneDown = 1760.0f;    // A6 (low = decrement)
    static constexpr uint32_t kToneDuration = 60;  // milliseconds (quick feedback)

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
