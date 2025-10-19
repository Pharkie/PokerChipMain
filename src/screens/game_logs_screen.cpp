#include "game_logs_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "screen_manager.hpp"
#include "game_active_screen.hpp"
#include "ui/ui_helpers.hpp"
#include "ui/ui_styles.hpp"

namespace {
constexpr const char* kLogTag = "game_logs_screen";
}

GameLogsScreen& GameLogsScreen::instance() {
    static GameLogsScreen instance;
    return instance;
}

void GameLogsScreen::create_widgets() {
    ESP_LOGI(kLogTag, "Creating widgets");

    lv_obj_t* scr = lv_scr_act();

    // Title background banner
    title_bg_ = lv_obj_create(scr);
    ui::styles::apply_title_bg(title_bg_);
    lv_obj_set_pos(title_bg_, -20, -10);

    // Title
    title_ = lv_label_create(scr);
    ui::styles::apply_title_text(title_);
    lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, 28);
    lv_label_set_text(title_, "Game Logs");

    // Create 5 labels for game entries (scrollable)
    for (int i = 0; i < kVisibleGames; i++) {
        log_labels_[i] = lv_label_create(scr);
        lv_obj_set_style_text_color(log_labels_[i], lv_color_hex(0xAAAAAA), LV_PART_MAIN);
        lv_obj_set_style_text_align(log_labels_[i], LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
        lv_obj_align(log_labels_[i], LV_ALIGN_TOP_LEFT, 10, 60 + (i * 26));
    }

    // Bottom button
    bottom_button_ = ui::helpers::create_button(scr);
    ui::styles::apply_bottom_button(bottom_button_);
    lv_obj_set_pos(bottom_button_, 0, 200);

    confirm_label_ = lv_label_create(bottom_button_);
    ui::styles::apply_bottom_button_label(confirm_label_);
    lv_label_set_text(confirm_label_, "Close");
    lv_obj_align(confirm_label_, LV_ALIGN_CENTER, 0, -12);
}

void GameLogsScreen::destroy_widgets() {
    ESP_LOGI(kLogTag, "Destroying widgets");

    for (int i = 0; i < kVisibleGames; i++) {
        if (log_labels_[i]) {
            lv_obj_del(log_labels_[i]);
            log_labels_[i] = nullptr;
        }
    }

    if (bottom_button_) lv_obj_del(bottom_button_);
    if (title_) lv_obj_del(title_);
    if (title_bg_) lv_obj_del(title_bg_);

    title_bg_ = nullptr;
    title_ = nullptr;
    bottom_button_ = nullptr;
    confirm_label_ = nullptr;
}

void GameLogsScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    scroll_offset_ = 0;
    load_records();
    update_display();

    // Setup event callback
    lv_obj_add_event_cb(bottom_button_, push_button_clicked_cb, LV_EVENT_CLICKED, this);
}

void GameLogsScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");
}

void GameLogsScreen::handle_encoder(int diff) {
    if (diff == 0 || record_count_ == 0) {
        return;
    }

    int prev_offset = scroll_offset_;
    scroll_offset_ += diff;

    // Clamp scroll offset
    if (scroll_offset_ < 0) {
        scroll_offset_ = 0;
        play_tone(kToneBoundary, kToneDuration);
    } else if (scroll_offset_ + kVisibleGames > record_count_) {
        scroll_offset_ = record_count_ - kVisibleGames;
        if (scroll_offset_ < 0) scroll_offset_ = 0;
        play_tone(kToneBoundary, kToneDuration);
    }

    if (scroll_offset_ != prev_offset) {
        play_tone(diff > 0 ? kToneUp : kToneDown, kToneDuration);
        update_display();
    }
}

void GameLogsScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, returning to game screen");

    // A6 → F6 chirp (dismiss)
    play_tone(1760.0f, 40);
    M5.delay(30);
    play_tone(1397.0f, 60);

    // Return to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

void GameLogsScreen::load_records() {
    record_count_ = storage::GameLog::instance().load_games(records_, 50);
    ESP_LOGI(kLogTag, "Loaded %d game records", record_count_);
}

void GameLogsScreen::update_display() {
    if (record_count_ == 0) {
        // No games saved yet
        lv_label_set_text(log_labels_[0], "No games logged yet");
        for (int i = 1; i < kVisibleGames; i++) {
            lv_label_set_text(log_labels_[i], "");
        }
        return;
    }

    // Display games in reverse order (most recent first)
    for (int i = 0; i < kVisibleGames; i++) {
        int record_idx = record_count_ - 1 - (scroll_offset_ + i);

        if (record_idx >= 0 && record_idx < record_count_) {
            const auto& rec = records_[record_idx];

            // Format time strings (M:SS or H:MM:SS - omit hours if zero)
            int g_hours = rec.game_seconds / 3600;
            int g_mins = (rec.game_seconds % 3600) / 60;
            int g_secs = rec.game_seconds % 60;

            int p_hours = rec.paused_seconds / 3600;
            int p_mins = (rec.paused_seconds % 3600) / 60;
            int p_secs = rec.paused_seconds % 60;

            char game_time_str[16];
            char paused_time_str[16];

            if (g_hours > 0) {
                snprintf(game_time_str, sizeof(game_time_str), "%d:%02d:%02d", g_hours, g_mins, g_secs);
            } else {
                snprintf(game_time_str, sizeof(game_time_str), "%d:%02d", g_mins, g_secs);
            }

            if (p_hours > 0) {
                snprintf(paused_time_str, sizeof(paused_time_str), "%d:%02d:%02d", p_hours, p_mins, p_secs);
            } else {
                snprintf(paused_time_str, sizeof(paused_time_str), "%d:%02d", p_mins, p_secs);
            }

            char label_text[80];
            snprintf(label_text, sizeof(label_text),
                     "#%lu: %s / %s R%d",
                     (unsigned long)rec.game_number,
                     game_time_str,
                     paused_time_str,
                     (int)rec.max_round);

            lv_label_set_text(log_labels_[i], label_text);
        } else {
            lv_label_set_text(log_labels_[i], "");
        }
    }
}

void GameLogsScreen::push_button_clicked_cb(lv_event_t* e) {
    GameLogsScreen* screen = static_cast<GameLogsScreen*>(lv_event_get_user_data(e));
    screen->handle_button_click();
}
