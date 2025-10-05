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

    // Show config screen widgets
    ui::hide_all_groups();
    ui::show_group(ui::groups().config_common);

    // Setup title - smaller font and higher position
    lv_label_set_text(ui().page_title, "Progression");
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 20);

    // Setup big number display (option name) - use 24pt purple font
    lv_obj_set_style_text_font(ui().big_number, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_number, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_number, lv_color_hex(0xFF00DC), LV_PART_MAIN);
    lv_obj_align(ui().big_number, LV_ALIGN_CENTER, 0, -30);

    // Setup push prompt background
    lv_obj_set_width(ui().pushtext_bg, 240);
    lv_obj_set_height(ui().pushtext_bg, 60);
    lv_obj_align(ui().pushtext_bg, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Setup push text
    lv_obj_set_style_text_align(ui().push_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().push_text, LV_ALIGN_BOTTOM_MID, 0, -24);

    // Setup down arrow
    lv_obj_set_style_text_align(ui().down_arrow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().down_arrow, LV_ALIGN_BOTTOM_MID, 0, -8);

    // Update display with initial selection
    update_display();

    // Make push button touchable
    lv_obj_add_event_cb(ui().pushtext_bg, push_button_clicked_cb, LV_EVENT_CLICKED, this);

    ESP_LOGI(kLogTag, "Initial selection: %s", kNames[selection_]);
}

void BlindProgressionScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");

    // Restore original colors for reused widgets (small_blind_active, big_blind_active)
    lv_obj_set_style_text_color(ui().small_blind_active, lv_color_hex(0x00FF46), LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_blind_active, lv_color_hex(0x00FBFF), LV_PART_MAIN);
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

    // Play confirmation tone
    play_tone(2637.0f, 120);  // E7

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
    lv_label_set_text(ui().big_number, kNames[selection_]);

    // Use small_blind_active for description - centered below name
    lv_label_set_text(ui().small_blind_active, kDescriptions[selection_]);
    lv_obj_set_style_text_font(ui().small_blind_active, LV_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().small_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().small_blind_active, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_align(ui().small_blind_active, LV_ALIGN_CENTER, 0, -10);
    set_visible(ui().small_blind_active, true);

    // Calculate dynamic game time estimate
    const GameState& game = GameState::instance();
    int estimated_rounds = calculate_estimated_rounds(kMultipliers[selection_]);
    int total_minutes = estimated_rounds * game.round_minutes();

    snprintf(game_time_buffer_, sizeof(game_time_buffer_),
             "Game time: ~%d mins", total_minutes);

    // Use big_blind_active for game time estimate - grayed out, below description
    lv_label_set_text(ui().big_blind_active, game_time_buffer_);
    lv_obj_set_style_text_font(ui().big_blind_active, LV_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_blind_active, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_align(ui().big_blind_active, LV_ALIGN_CENTER, 0, 20);
    set_visible(ui().big_blind_active, true);
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
    set_visible(ui().info_overlay, true);
    play_tone(1500.0f, 80);
}

void BlindProgressionScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(ui().info_overlay, false);
    play_tone(1200.0f, 80);
}
