#include "small_blind_screen.hpp"

#include <algorithm>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "round_minutes_screen.hpp"

namespace {
constexpr const char* kLogTag = "small_blind_screen";
}

SmallBlindScreen& SmallBlindScreen::instance() {
    static SmallBlindScreen instance;
    return instance;
}

void SmallBlindScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Reset to minimum value
    value_ = kMin;

    // Hide boot logo
    set_visible(ui().logo, false);

    // Setup title
    lv_label_set_text(ui().page_title, "Starting small blinds");
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 40);
    set_visible(ui().page_title, true);

    // Setup big number display - 48pt purple font
    lv_obj_set_style_text_font(ui().big_number, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui().big_number, lv_color_hex(0xFF00DC), LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_number, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().big_number, LV_ALIGN_CENTER, 0, 0);
    update_display();
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
    lv_obj_align(ui().down_arrow, LV_ALIGN_BOTTOM_MID, 0, -4);
    set_visible(ui().down_arrow, true);

    // Hide game active widgets
    set_visible(ui().small_blind_active, false);
    set_visible(ui().big_blind_active, false);
    set_visible(ui().active_small_blind_label, false);
    set_visible(ui().active_big_blind_label, false);
    set_visible(ui().elapsed_mins, false);
    set_visible(ui().elapsed_secs, false);
    set_visible(ui().mins_label, false);
    set_visible(ui().secs_label, false);

    // Show info button
    set_visible(ui().info_button, true);
    lv_obj_add_event_cb(ui().info_button, info_button_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(ui().info_overlay, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(ui().info_close_button, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
}

void SmallBlindScreen::handle_encoder(int diff) {
    if (diff == 0 || is_modal_blocking()) {
        return;  // Ignore encoder when modal overlay is shown
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
        ESP_LOGI(kLogTag, "Small blind -> %d", value_);
    } else if (boundary) {
        play_tone(kToneBoundary, kToneDuration);
        ESP_LOGI(kLogTag, "Boundary hit at %d", value_);
    }
}

void SmallBlindScreen::handle_button_click() {
    // If modal overlay is blocking, close it instead of advancing
    if (is_modal_blocking()) {
        hide_info();
        return;
    }

    ESP_LOGI(kLogTag, "Button clicked, value=%d", value_);

    // Save to game state (big_blind automatically set to 2x small_blind)
    GameState::instance().set_small_blind(value_);

    // Play confirmation tone (G7)
    play_tone(2960.0f, 120);

    // Transition to RoundMinutesScreen
    ScreenManager::instance().transition_to(&RoundMinutesScreen::instance());
}

void SmallBlindScreen::update_display() {
    lv_label_set_text_fmt(ui().big_number, "%d", value_);
}

void SmallBlindScreen::info_button_clicked_cb(lv_event_t* e) {
    SmallBlindScreen* screen = static_cast<SmallBlindScreen*>(lv_event_get_user_data(e));
    screen->show_info();
}

void SmallBlindScreen::info_overlay_clicked_cb(lv_event_t* e) {
    SmallBlindScreen* screen = static_cast<SmallBlindScreen*>(lv_event_get_user_data(e));
    screen->hide_info();
}

void SmallBlindScreen::show_info() {
    ESP_LOGI(kLogTag, "Showing info overlay");
    set_visible(ui().info_overlay, true);
    play_tone(1500.0f, 80);
}

void SmallBlindScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(ui().info_overlay, false);
    play_tone(1200.0f, 80);
}
