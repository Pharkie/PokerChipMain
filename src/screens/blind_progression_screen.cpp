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

    // Hide boot logo
    set_visible(ui().logo, false);

    // Setup title - smaller font and higher position
    lv_label_set_text(ui().page_title, "Progression");
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 20);
    set_visible(ui().page_title, true);

    // Setup big number display (option name) - use 24pt font for "STANDARD"
    lv_obj_set_style_text_font(ui().big_number, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_number, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_number, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(ui().big_number, LV_ALIGN_CENTER, 0, -30);
    set_visible(ui().big_number, true);

    // Setup push prompt background
    lv_obj_set_width(ui().pushtext_bg, 240);
    lv_obj_set_height(ui().pushtext_bg, 60);
    lv_obj_align(ui().pushtext_bg, LV_ALIGN_BOTTOM_MID, 0, 0);
    set_visible(ui().pushtext_bg, true);

    // Setup push text
    lv_obj_set_style_text_align(ui().push_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().push_text, LV_ALIGN_BOTTOM_MID, 0, -24);
    set_visible(ui().push_text, true);

    // Setup down arrow
    lv_obj_set_style_text_align(ui().down_arrow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().down_arrow, LV_ALIGN_BOTTOM_MID, 0, -8);
    set_visible(ui().down_arrow, true);

    // Update display with initial selection
    update_display();

    ESP_LOGI(kLogTag, "Initial selection: %s", kNames[selection_]);
}

void BlindProgressionScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");
    set_visible(ui().page_title, false);
    set_visible(ui().big_number, false);
    set_visible(ui().pushtext_bg, false);
    set_visible(ui().push_text, false);
    set_visible(ui().down_arrow, false);

    // Hide and restore original colors for reused widgets
    set_visible(ui().small_blind_active, false);
    set_visible(ui().big_blind_active, false);
    lv_obj_set_style_text_color(ui().small_blind_active, lv_color_hex(0x00FF46), LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_blind_active, lv_color_hex(0x00FBFF), LV_PART_MAIN);
}

void BlindProgressionScreen::handle_encoder(int diff) {
    int prev_selection = selection_;
    selection_ += diff;

    // Wrap around
    if (selection_ < 0) {
        selection_ = kOptionCount - 1;
        play_tone(kToneBoundary, kToneDuration);
    } else if (selection_ >= kOptionCount) {
        selection_ = 0;
        play_tone(kToneBoundary, kToneDuration);
    } else {
        // Normal selection change
        play_tone(diff > 0 ? kToneUp : kToneDown, kToneDuration);
    }

    if (selection_ != prev_selection) {
        update_display();
        ESP_LOGI(kLogTag, "Selection changed to: %s", kNames[selection_]);
    }
}

void BlindProgressionScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, selected: %s (multiplier: %.2f)",
             kNames[selection_], kMultipliers[selection_]);

    // Store selected multiplier in game state
    GameState& game = GameState::instance();
    game.blind_multiplier = kMultipliers[selection_];

    // Initialize timer for first round
    game.seconds_remaining = game.round_minutes * 60;

    // Play confirmation tone
    play_tone(2637.0f, 120);  // E7

    // Transition to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

void BlindProgressionScreen::update_display() {
    // Show option name (TURBO/STANDARD/RELAXED) - white text
    lv_label_set_text(ui().big_number, kNames[selection_]);

    // Use small_blind_active for description - centered below name
    lv_label_set_text(ui().small_blind_active, kDescriptions[selection_]);
    lv_obj_set_style_text_font(ui().small_blind_active, LV_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().small_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().small_blind_active, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_align(ui().small_blind_active, LV_ALIGN_CENTER, 0, -10);
    set_visible(ui().small_blind_active, true);

    // Use big_blind_active for game time estimate - grayed out, below description
    lv_label_set_text(ui().big_blind_active, kGameTimes[selection_]);
    lv_obj_set_style_text_font(ui().big_blind_active, LV_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_blind_active, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_align(ui().big_blind_active, LV_ALIGN_CENTER, 0, 20);
    set_visible(ui().big_blind_active, true);
}
