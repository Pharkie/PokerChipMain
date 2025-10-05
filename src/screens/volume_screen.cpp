#include "volume_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "screen_manager.hpp"
#include "game_active_screen.hpp"
#include "storage/nvs_storage.hpp"

namespace {
constexpr const char* kLogTag = "volume_screen";
}

VolumeScreen& VolumeScreen::instance() {
    static VolumeScreen instance;
    return instance;
}

void VolumeScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Load saved volume or use current value
    value_ = storage::NVSStorage::instance().load_volume(5);
    apply_volume();

    // Show config screen widgets
    ui::hide_all_groups();
    ui::show_group(ui::groups().config_common);

    // Setup title
    lv_label_set_text(ui().page_title, "Volume");
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

    // Setup push text (text already set to "Confirm" in ui_root.cpp)
    lv_obj_set_style_text_align(ui().push_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().push_text, LV_ALIGN_BOTTOM_MID, 0, -24);

    // Setup down arrow
    lv_obj_align(ui().down_arrow, LV_ALIGN_BOTTOM_MID, 0, -4);

    // Make push button touchable
    lv_obj_add_event_cb(ui().pushtext_bg, push_button_clicked_cb, LV_EVENT_CLICKED, this);
}

void VolumeScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");

    // Remove event callback to prevent duplicates on re-entry
    lv_obj_remove_event_cb(ui().pushtext_bg, push_button_clicked_cb);
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

    // Play confirmation tone (C#7 - settings action)
    play_tone(2217.0f, 100);

    // Return to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

void VolumeScreen::update_display() {
    lv_label_set_text_fmt(ui().big_number, "%d", value_);
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
