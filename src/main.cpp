// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#include <M5Unified.hpp>
#include <lvgl.h>
#include <esp_log.h>

#include "M5Dial-LVGL.h"
#include "ui/ui_root.hpp"
#include "ui/ui_assets.hpp"
#include "input/encoder_input.hpp"
#include "hardware/button.hpp"
#include "hardware/encoder.hpp"
#include "screens/screen_manager.hpp"
#include "screens/small_blind_screen.hpp"

static const char *TAG = "poker_chip";

// Hardware instances
static hardware::Button* g_btnA = nullptr;

extern const uint8_t _binary_src_images_riccy_png_start[];
extern const uint8_t _binary_src_images_riccy_png_end[];

void setup()
{
    ESP_LOGI(TAG, "Program starting");

    M5.begin();
    M5.Display.fillScreen(TFT_BLACK);

    // Show splash screen
    const uint8_t *splash_data = _binary_src_images_riccy_png_start;
    const size_t splash_size = static_cast<size_t>(_binary_src_images_riccy_png_end - _binary_src_images_riccy_png_start);
    if (!M5.Display.drawPng(splash_data, splash_size, 0, 0))
    {
        ESP_LOGW(TAG, "Failed to draw embedded splash image");
    }

    // Startup sound sequence (E7 → E8)
    const uint32_t tone1 = 120;
    const uint32_t gap = 80;
    const uint32_t tone2 = 150;

    M5.Speaker.tone(3135.0f, tone1);
    M5.delay(tone1);
    M5.delay(gap);
    M5.Speaker.tone(4186.0f, tone2);
    const uint32_t elapsed = tone1 + gap + tone2;
    if (elapsed < 2000)
    {
        M5.delay(2000 - elapsed);
    }
    M5.Display.fillScreen(TFT_BLACK);

    // Initialize LVGL and UI
    m5dial_lvgl_init(false);
    ui::ui_init();
    ui::assets::init();
    encoder_input::init(ui::get().focus_proxy);

    // Initialize hardware modules
    g_btnA = new hardware::Button(GPIO_NUM_42, 100, 2000);  // 100ms debounce, 2s long press
    g_btnA->on_short_press([]() {
        ScreenManager::instance().handle_button_click();
    });
    g_btnA->on_long_press([]() {
        M5.Power.powerOff();
    });

    hardware::Encoder::instance().on_rotation([](int delta) {
        ScreenManager::instance().handle_encoder(delta);
    });

    // Show logo and start with small blind screen
    lv_obj_clear_flag(ui::get().logo, LV_OBJ_FLAG_HIDDEN);
    ScreenManager::instance().init();
    ScreenManager::instance().transition_to(&SmallBlindScreen::instance());
}

void loop()
{
    M5.update();
    m5dial_lvgl_next();

    // Update hardware modules
    if (g_btnA) {
        g_btnA->update();
    }

    // Update active screen
    ScreenManager::instance().tick();

    M5.delay(5);
}
