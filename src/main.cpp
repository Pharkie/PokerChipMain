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

void setup()
{
    ESP_LOGI(TAG, "Program starting");

    m5dial_lvgl_init();
    ui::ui_init();
    encoder_input::init(ui::get().focus_proxy);
    app_tasks::init();
}

void loop()
{
    m5dial_lvgl_next();
    app_tasks::tick();
}

extern "C"
{
    void app_main()
    {
        setup();
        while (true)
        {
            loop();
        }
    }
}
