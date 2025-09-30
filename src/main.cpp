// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#include <M5Unified.hpp>
#include <lvgl.h>
#include <esp_log.h>

#include "M5Dial-LVGL.h"
#include "ui/ui_root.hpp"
#include "input/encoder_input.hpp"
#include "tasks/app_tasks.hpp"

static const char *TAG = "poker_chip";

extern const uint8_t _binary_src_images_riccy_png_start[];
extern const uint8_t _binary_src_images_riccy_png_end[];

void setup()
{
    ESP_LOGI(TAG, "Program starting");

    M5.begin();
    M5.Display.fillScreen(TFT_BLACK);

    const uint8_t *splash_data = _binary_src_images_riccy_png_start;
    const size_t splash_size = static_cast<size_t>(_binary_src_images_riccy_png_end - _binary_src_images_riccy_png_start);
    if (!M5.Display.drawPng(splash_data, splash_size, 0, 0))
    {
        ESP_LOGW(TAG, "Failed to draw embedded splash image");
    }

    M5.delay(2000);
    M5.Display.fillScreen(TFT_BLACK);

    m5dial_lvgl_init(false);
    ui::ui_init();
    encoder_input::init(ui::get().focus_proxy);
    app_tasks::init();
}

void loop()
{
    M5.update();
    m5dial_lvgl_next();
    app_tasks::tick();
    M5.delay(5);
}
