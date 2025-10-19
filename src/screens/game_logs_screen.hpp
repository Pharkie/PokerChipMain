#pragma once

#include "screen.hpp"
#include "storage/game_log.hpp"

/// Game logs viewer screen showing saved game records.
/// Displays list of saved games with game time, paused time, and max round.
class GameLogsScreen : public Screen {
public:
    /// Get the singleton instance.
    static GameLogsScreen& instance();

    // Screen interface implementation
    void create_widgets() override;
    void destroy_widgets() override;
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;

private:
    GameLogsScreen() = default;

    // Widget handles
    lv_obj_t* title_bg_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* log_labels_[5] = {nullptr};  // Show up to 5 games at once
    lv_obj_t* bottom_button_ = nullptr;
    lv_obj_t* confirm_label_ = nullptr;

    storage::GameRecord records_[50];
    int record_count_ = 0;
    int scroll_offset_ = 0;  // Index of first visible game

    static constexpr int kVisibleGames = 5;
    static constexpr float kToneUp = 2637.0f;      // E7
    static constexpr float kToneDown = 1760.0f;    // A6
    static constexpr float kToneBoundary = 1245.0f; // D#6 (boundary)
    static constexpr uint32_t kToneDuration = 60;

    void load_records();
    void update_display();

    // Touch handler
    static void push_button_clicked_cb(lv_event_t* e);
};
