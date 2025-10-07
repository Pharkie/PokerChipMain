#include "volume_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "screen_manager.hpp"
#include "game_active_screen.hpp"
#include "storage/nvs_storage.hpp"
#include "ui/ui_helpers.hpp"
#include "ui/ui_styles.hpp"

namespace {
constexpr const char* kLogTag = "volume_screen";
}

VolumeScreen& VolumeScreen::instance() {
    static VolumeScreen instance;
    return instance;
}

void VolumeScreen::create_widgets() {
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
    lv_label_set_text(title_, "Volume");

    // Big number (volume level 0-10)
    big_number_ = lv_label_create(scr);
    ui::styles::apply_big_number(big_number_);
    lv_obj_align(big_number_, LV_ALIGN_CENTER, 0, 0);

    // Bottom button
    bottom_button_ = ui::helpers::create_button(scr);
    ui::styles::apply_bottom_button(bottom_button_);
    lv_obj_set_pos(bottom_button_, 0, 200);

    confirm_label_ = lv_label_create(bottom_button_);
    ui::styles::apply_bottom_button_label(confirm_label_);
    lv_label_set_text(confirm_label_, "Confirm");
    lv_obj_align(confirm_label_, LV_ALIGN_CENTER, 0, -12);
}

void VolumeScreen::destroy_widgets() {
    ESP_LOGI(kLogTag, "Destroying widgets");

    if (bottom_button_) lv_obj_del(bottom_button_);
    if (big_number_) lv_obj_del(big_number_);
    if (title_) lv_obj_del(title_);
    if (title_bg_) lv_obj_del(title_bg_);

    title_bg_ = nullptr;
    title_ = nullptr;
    big_number_ = nullptr;
    bottom_button_ = nullptr;
    confirm_label_ = nullptr;
}

void VolumeScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Load saved volume or use current value
    value_ = storage::NVSStorage::instance().load_volume(5);
    apply_volume();
    update_display();

    // Setup event callback
    lv_obj_add_event_cb(bottom_button_, push_button_clicked_cb, LV_EVENT_CLICKED, this);
}

void VolumeScreen::on_exit() {
    // Event callbacks will be destroyed with widgets
}

void VolumeScreen::handle_encoder(int diff) {
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
        apply_volume();
        update_display();
        play_tone(step > 0 ? kToneUp : kToneDown, kToneDuration);
        ESP_LOGI(kLogTag, "Volume -> %d", value_);
    } else if (boundary) {
        play_tone(kToneBoundary, 80);  // Boundary slightly longer
        ESP_LOGI(kLogTag, "Boundary hit at %d", value_);
    }
}

void VolumeScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked, saving volume=%d", value_);

    // Save to NVS
    storage::NVSStorage::instance().save_volume(value_);

    // Play confirmation tone (B6 → C7 → B6 wobble - playful volume saved)
    play_tone(1976.0f, 60);  // B6
    M5.delay(40);
    play_tone(2093.0f, 60);  // C7
    M5.delay(40);
    play_tone(1976.0f, 70);  // B6

    // Return to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

void VolumeScreen::update_display() {
    lv_label_set_text_fmt(big_number_, "%d", value_);
}

void VolumeScreen::push_button_clicked_cb(lv_event_t* e) {
    VolumeScreen* screen = static_cast<VolumeScreen*>(lv_event_get_user_data(e));
    screen->handle_button_click();
}

void VolumeScreen::apply_volume() {
    // Convert 0-10 scale to 0-255 M5.Speaker range
    uint8_t speaker_volume = (value_ * 255) / 10;
    M5.Speaker.setVolume(speaker_volume);
    ESP_LOGI(kLogTag, "Applied volume: %d (speaker: %d/255)", value_, speaker_volume);
}
