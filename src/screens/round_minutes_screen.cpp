#include "round_minutes_screen.hpp"

#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "blind_progression_screen.hpp"

namespace {
constexpr const char* kLogTag = "round_minutes_screen";
}

RoundMinutesScreen& RoundMinutesScreen::instance() {
    static RoundMinutesScreen instance;
    return instance;
}

void RoundMinutesScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Reset to default value
    value_ = 10;

    // Show config screen widgets
    ui::hide_all_groups();
    ui::show_group(ui::groups().config_common);

    // Setup title
    lv_label_set_text(ui().page_title, "Mins between rounds");
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 40);

    // Setup big number display - 48pt purple font
    lv_obj_set_style_text_font(ui().big_number, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_number, lv_color_hex(0xFF00DC), LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_number, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().big_number, LV_ALIGN_CENTER, 0, 0);
    update_display();

    // Setup push prompt background
    lv_obj_set_width(ui().pushtext_bg, 240);
    lv_obj_set_height(ui().pushtext_bg, 60);
    lv_obj_align(ui().pushtext_bg, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Setup push text
    lv_obj_set_style_text_align(ui().push_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().push_text, LV_ALIGN_BOTTOM_MID, 0, -24);

    // Setup down arrow
    lv_obj_align(ui().down_arrow, LV_ALIGN_BOTTOM_MID, 0, -4);
}

void RoundMinutesScreen::handle_encoder(int diff) {
    if (diff == 0) {
        return;
    }

    // Normalize to step delta
    int step = (diff > 0) ? 1 : -1;
    int next = value_ + step * kStep;
    bool boundary = false;

    // Clamp to range
    if (next < kMin) {
        next = kMin;
        boundary = true;
    }
    if (next > kMax) {
        next = kMax;
        boundary = true;
    }

    // Update value and play feedback
    if (next != value_) {
        value_ = next;
        update_display();
        play_tone(step > 0 ? kToneUp : kToneDown, kToneDuration);
        ESP_LOGI(kLogTag, "Round minutes -> %d", value_);
    } else if (boundary) {
        play_tone(kToneBoundary, kToneDuration);
        ESP_LOGI(kLogTag, "Boundary hit at %d", value_);
    }
}

void RoundMinutesScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, value=%d", value_);

    // Save to game state
    GameState::instance().set_round_minutes(value_);

    // Play confirmation tone (C8)
    play_tone(4186.0f, 120);

    // Transition to Blind Progression Screen
    ScreenManager::instance().transition_to(&BlindProgressionScreen::instance());
}

void RoundMinutesScreen::update_display() {
    lv_label_set_text_fmt(ui().big_number, "%d", value_);
}

void RoundMinutesScreen::info_button_clicked_cb(lv_event_t* e) {
    RoundMinutesScreen* screen = static_cast<RoundMinutesScreen*>(lv_event_get_user_data(e));
    screen->show_info();
}

void RoundMinutesScreen::info_overlay_clicked_cb(lv_event_t* e) {
    RoundMinutesScreen* screen = static_cast<RoundMinutesScreen*>(lv_event_get_user_data(e));
    screen->hide_info();
}

void RoundMinutesScreen::show_info() {
    ESP_LOGI(kLogTag, "Showing info overlay");
    set_visible(ui().info_overlay, true);
    play_tone(1500.0f, 80);
}

void RoundMinutesScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(ui().info_overlay, false);
    play_tone(1200.0f, 80);
}
