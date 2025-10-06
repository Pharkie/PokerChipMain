#include "blind_progression_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "game_active_screen.hpp"

namespace {
constexpr const char* kLogTag = "blind_progression_screen";
}

BlindProgressionScreen& BlindProgressionScreen::instance() {
    static BlindProgressionScreen instance;
    return instance;
}

void BlindProgressionScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Default to STANDARD (first option)
    selection_ = 0;

    // Show mode screen widgets
    ui::hide_all_groups();
    ui::show_group(ui::groups().mode_screen);

    lv_label_set_text(ui().page_title, "Mode");
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    lv_obj_set_style_text_align(ui().mode_name, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // Update display with initial selection
    update_display();

    // Make push button touchable
    lv_obj_add_event_cb(ui().bottom_button_bg, push_button_clicked_cb, LV_EVENT_CLICKED, this);

    ESP_LOGI(kLogTag, "Initial selection: %s", kNames[selection_]);
}

void BlindProgressionScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");

    // Remove event callback to prevent duplicates on re-entry
    lv_obj_remove_event_cb(ui().bottom_button_bg, push_button_clicked_cb);
}

void BlindProgressionScreen::handle_encoder(int diff) {
    int prev_selection = selection_;
    selection_ += diff;

    // Wrap around (all choices are valid, no boundaries)
    if (selection_ < 0) {
        selection_ = kOptionCount - 1;
    } else if (selection_ >= kOptionCount) {
        selection_ = 0;
    }

    if (selection_ != prev_selection) {
        // Play consistent tone for any valid selection change
        play_tone(diff > 0 ? kToneUp : kToneDown, kToneDuration);
        update_display();
        ESP_LOGI(kLogTag, "Selection changed to: %s", kNames[selection_]);
    }
}

void BlindProgressionScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, selected: %s (multiplier: %.2f)",
             kNames[selection_], kMultipliers[selection_]);

    // Store selected multiplier in game state
    GameState& game = GameState::instance();
    game.set_blind_multiplier(kMultipliers[selection_]);

    // Initialize timer for first round
    game.set_seconds_remaining(game.round_minutes() * 60);

    // Play confirmation tone (A7 → A7 double beep - climax, game starting!)
    play_tone(3520.0f, 90);
    M5.delay(60);
    play_tone(3520.0f, 90);

    // Transition to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

int BlindProgressionScreen::calculate_estimated_rounds(float multiplier) const {
    const GameState& game = GameState::instance();
    int small_blind = game.small_blind();

    // Simulate blind progression until endgame
    // Game typically ends when average stack gets very short relative to blinds
    // Conservative: when BB reaches ~25% of starting stack (500 chips)
    // At this point, players are playing push/fold poker
    int critical_big_blind = kStartingStack / 4;  // 500 for 2000 stack
    int rounds = 0;
    int current_sb = small_blind;

    while (current_sb * 2 < critical_big_blind && rounds < 30) {
        rounds++;
        current_sb = static_cast<int>(current_sb * multiplier);
    }

    return rounds;
}

void BlindProgressionScreen::update_display() {
    // Show option name (TURBO/STANDARD/RELAXED) - purple text
    lv_label_set_text(ui().mode_name, kNames[selection_]);

    // Show description - centered below name
    lv_label_set_text(ui().mode_description, kDescriptions[selection_]);
    lv_obj_set_style_text_align(ui().mode_description, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // Calculate dynamic game time estimate
    const GameState& game = GameState::instance();
    int estimated_rounds = calculate_estimated_rounds(kMultipliers[selection_]);
    int total_minutes = estimated_rounds * game.round_minutes();

    snprintf(game_time_buffer_, sizeof(game_time_buffer_),
             "Game time: ~%d mins", total_minutes);

    // Show game time estimate - grayed out, below description
    lv_label_set_text(ui().mode_game_time, game_time_buffer_);
    lv_obj_set_style_text_align(ui().mode_game_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
}

void BlindProgressionScreen::push_button_clicked_cb(lv_event_t* e) {
    BlindProgressionScreen* screen = static_cast<BlindProgressionScreen*>(lv_event_get_user_data(e));
    screen->handle_button_click();
}

void BlindProgressionScreen::info_button_clicked_cb(lv_event_t* e) {
    BlindProgressionScreen* screen = static_cast<BlindProgressionScreen*>(lv_event_get_user_data(e));
    screen->show_info();
}

void BlindProgressionScreen::info_overlay_clicked_cb(lv_event_t* e) {
    BlindProgressionScreen* screen = static_cast<BlindProgressionScreen*>(lv_event_get_user_data(e));
    screen->hide_info();
}

void BlindProgressionScreen::show_info() {
    ESP_LOGI(kLogTag, "Showing info overlay");
    ui::show_group(ui::groups().info);
    // F#7 → A7 chirp (playful upward)
    play_tone(2794.0f, 40);  // F#7
    M5.delay(40);
    play_tone(3520.0f, 60);  // A7
}

void BlindProgressionScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    ui::hide_group(ui::groups().info);
    // A6 → F6 chirp (playful downward)
    play_tone(1760.0f, 40);  // A6
    M5.delay(40);
    play_tone(1397.0f, 60);  // F6
}
