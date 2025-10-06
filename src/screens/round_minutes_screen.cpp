#include "round_minutes_screen.hpp"

#include <M5Unified.hpp>
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

    // Setup push text

    // Make push button touchable
    lv_obj_add_event_cb(ui().bottom_button_bg, push_button_clicked_cb, LV_EVENT_CLICKED, this);
}

void RoundMinutesScreen::on_exit() {
    // Remove event callback to prevent duplicates on re-entry
    lv_obj_remove_event_cb(ui().bottom_button_bg, push_button_clicked_cb);
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
        play_tone(kToneBoundary, 80);  // Boundary slightly longer
        ESP_LOGI(kLogTag, "Boundary hit at %d", value_);
    }
}

void RoundMinutesScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, value=%d", value_);

    // Save to game state
    GameState::instance().set_round_minutes(value_);

    // Play confirmation tone (F7 → F7 double beep - build excitement)
    play_tone(2793.0f, 80);
    M5.delay(60);
    play_tone(2793.0f, 80);

    // Transition to Blind Progression Screen
    ScreenManager::instance().transition_to(&BlindProgressionScreen::instance());
}

void RoundMinutesScreen::update_display() {
    lv_label_set_text_fmt(ui().big_number, "%d", value_);
}

void RoundMinutesScreen::push_button_clicked_cb(lv_event_t* e) {
    RoundMinutesScreen* screen = static_cast<RoundMinutesScreen*>(lv_event_get_user_data(e));
    screen->handle_button_click();
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
    // F#7 → A7 chirp (playful upward)
    play_tone(2794.0f, 40);  // F#7
    M5.delay(40);
    play_tone(3520.0f, 60);  // A7
}

void RoundMinutesScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(ui().info_overlay, false);
    // A6 → F6 chirp (playful downward)
    play_tone(1760.0f, 40);  // A6
    M5.delay(40);
    play_tone(1397.0f, 60);  // F6
}
