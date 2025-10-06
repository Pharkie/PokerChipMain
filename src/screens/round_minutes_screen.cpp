#include "round_minutes_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "blind_progression_screen.hpp"
#include "ui/ui_styles.hpp"

namespace {
constexpr const char* kLogTag = "round_minutes_screen";
}

RoundMinutesScreen& RoundMinutesScreen::instance() {
    static RoundMinutesScreen instance;
    return instance;
}

void RoundMinutesScreen::create_widgets() {
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
    lv_label_set_text(title_, "Round length (mins)");

    // Big number
    big_number_ = lv_label_create(scr);
    ui::styles::apply_big_number(big_number_);
    lv_obj_align(big_number_, LV_ALIGN_CENTER, 0, 0);

    // Bottom button
    bottom_button_ = lv_button_create(scr);
    ui::styles::apply_bottom_button(bottom_button_);
    lv_obj_set_pos(bottom_button_, 0, 200);
    lv_obj_add_flag(bottom_button_, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_clear_flag(bottom_button_, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_remove_state(bottom_button_, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(bottom_button_, 0, LV_PART_MAIN);

    confirm_label_ = lv_label_create(bottom_button_);
    ui::styles::apply_bottom_button_label(confirm_label_);
    lv_label_set_text(confirm_label_, "Confirm");
    lv_obj_align(confirm_label_, LV_ALIGN_CENTER, 0, -12);

    // Info button
    info_button_ = lv_button_create(scr);
    ui::styles::apply_info_button(info_button_);
    lv_obj_set_pos(info_button_, 185, 103);
    lv_obj_add_flag(info_button_, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_clear_flag(info_button_, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_remove_state(info_button_, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(info_button_, 0, LV_PART_MAIN);
    lv_obj_t* info_label = lv_label_create(info_button_);
    lv_label_set_text(info_label, "i");
    lv_obj_set_style_text_color(info_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(info_label);

    // Info overlay
    info_overlay_ = lv_obj_create(scr);
    ui::styles::apply_overlay_bg(info_overlay_);
    lv_obj_set_pos(info_overlay_, 0, 0);
    lv_obj_add_flag(info_overlay_, LV_OBJ_FLAG_CLICKABLE);

    info_title_bg_ = lv_obj_create(info_overlay_);
    ui::styles::apply_title_bg(info_title_bg_);
    lv_obj_set_pos(info_title_bg_, -20, -10);

    info_title_ = lv_label_create(info_overlay_);
    ui::styles::apply_title_text(info_title_);
    lv_obj_align(info_title_, LV_ALIGN_TOP_MID, 0, 28);
    lv_label_set_text(info_title_, "Chip Breakdown");

    info_blue_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_blue_, "16 x Blue (25) = 400");
    lv_obj_set_style_text_color(info_blue_, lv_color_hex(0x4488FF), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_blue_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_blue_, LV_ALIGN_CENTER, 0, -35);

    info_white_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_white_, "20 x White (50) = 1000");
    lv_obj_set_style_text_color(info_white_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_white_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_white_, LV_ALIGN_CENTER, 0, -10);

    info_red_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_red_, "6 x Red (100) = 600");
    lv_obj_set_style_text_color(info_red_, lv_color_hex(0xFF4444), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_red_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_red_, LV_ALIGN_CENTER, 0, 15);

    info_stack_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_stack_, "Total stack: 2000");
    lv_obj_set_style_text_color(info_stack_, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_stack_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_stack_, LV_ALIGN_CENTER, 0, 45);

    info_close_button_ = lv_button_create(info_overlay_);
    ui::styles::apply_bottom_button(info_close_button_);
    lv_obj_set_pos(info_close_button_, 0, 200);
    lv_obj_add_flag(info_close_button_, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_clear_flag(info_close_button_, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_remove_state(info_close_button_, LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(info_close_button_, 0, LV_PART_MAIN);

    info_close_label_ = lv_label_create(info_close_button_);
    ui::styles::apply_bottom_button_label(info_close_label_);
    lv_label_set_text(info_close_label_, "Close");
    lv_obj_align(info_close_label_, LV_ALIGN_CENTER, 0, -12);

    lv_obj_add_flag(info_overlay_, LV_OBJ_FLAG_HIDDEN);
}

void RoundMinutesScreen::destroy_widgets() {
    ESP_LOGI(kLogTag, "Destroying widgets");

    if (info_overlay_) lv_obj_del(info_overlay_);
    if (info_button_) lv_obj_del(info_button_);
    if (bottom_button_) lv_obj_del(bottom_button_);
    if (big_number_) lv_obj_del(big_number_);
    if (title_) lv_obj_del(title_);
    if (title_bg_) lv_obj_del(title_bg_);

    title_bg_ = nullptr;
    title_ = nullptr;
    big_number_ = nullptr;
    bottom_button_ = nullptr;
    confirm_label_ = nullptr;
    info_button_ = nullptr;
    info_overlay_ = nullptr;
    info_title_bg_ = nullptr;
    info_title_ = nullptr;
    info_blue_ = nullptr;
    info_white_ = nullptr;
    info_red_ = nullptr;
    info_stack_ = nullptr;
    info_close_button_ = nullptr;
    info_close_label_ = nullptr;
}

void RoundMinutesScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Reset to default value
    value_ = 10;
    update_display();

    // Setup event callbacks
    lv_obj_add_event_cb(info_button_, info_button_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(info_overlay_, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(info_close_button_, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(bottom_button_, push_button_clicked_cb, LV_EVENT_CLICKED, this);
}

void RoundMinutesScreen::on_exit() {
    // Event callbacks will be destroyed with widgets
}

void RoundMinutesScreen::handle_encoder(int diff) {
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
        ESP_LOGI(kLogTag, "Round minutes -> %d", value_);
    } else if (boundary) {
        play_tone(kToneBoundary, 80);  // Boundary slightly longer
        ESP_LOGI(kLogTag, "Boundary hit at %d", value_);
    }
}

void RoundMinutesScreen::handle_button_click() {
    // If info overlay is visible, close it instead of confirming
    if (is_modal_blocking()) {
        hide_info();
        return;
    }

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
    lv_label_set_text_fmt(big_number_, "%d", value_);
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
    set_visible(info_overlay_, true);
    // F#7 → A7 chirp (playful upward)
    play_tone(2794.0f, 40);  // F#7
    M5.delay(40);
    play_tone(3520.0f, 60);  // A7
}

void RoundMinutesScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(info_overlay_, false);
    // A6 → F6 chirp (playful downward)
    play_tone(1760.0f, 40);  // A6
    M5.delay(40);
    play_tone(1397.0f, 60);  // F6
}

bool RoundMinutesScreen::is_modal_blocking() const {
    return info_overlay_ && !lv_obj_has_flag(info_overlay_, LV_OBJ_FLAG_HIDDEN);
}
