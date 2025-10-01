#include "app_tasks.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include <lvgl.h>
#include <driver/gpio.h>
#include <esp_timer.h>

#include "input/encoder_input.hpp"
#include "screens/screen_manager.hpp"
#include "screens/small_blind_screen.hpp"
#include "ui/ui_assets.hpp"
#include "ui/ui_root.hpp"

namespace app_tasks
{
    namespace
    {
        constexpr const char *kLogTag = "app_tasks";
        constexpr gpio_num_t kBtnAPin = GPIO_NUM_42;
        constexpr uint32_t kLongPressMs = 2000;  // 2 seconds to power off

    } // namespace

    void init()
    {
        ui::assets::init();

        gpio_config_t btn_cfg = {};
        btn_cfg.pin_bit_mask = 1ULL << kBtnAPin;
        btn_cfg.mode = GPIO_MODE_INPUT;
        btn_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
        btn_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        btn_cfg.intr_type = GPIO_INTR_DISABLE;
        gpio_config(&btn_cfg);

        const auto &handles = ui::get();
        lv_obj_clear_flag(handles.logo, LV_OBJ_FLAG_HIDDEN);

        // Initialize screen manager and start with small blind screen
        ScreenManager::instance().init();
        ScreenManager::instance().transition_to(&SmallBlindScreen::instance());
    }

    void tick()
    {
        static bool prev_pressed = false;
        static uint32_t press_start_ms = 0;

        bool pressed = gpio_get_level(kBtnAPin) == 0;
        uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);

        if (pressed && !prev_pressed)
        {
            // Button just pressed
            ESP_LOGI(kLogTag, "BtnA pressed");
            press_start_ms = now_ms;
        }
        else if (pressed && prev_pressed)
        {
            // Button being held - check duration
            uint32_t held_ms = now_ms - press_start_ms;
            if (held_ms >= kLongPressMs)
            {
                ESP_LOGI(kLogTag, "Long press detected (%lu ms) - powering off", held_ms);
                M5.Power.powerOff();  // Sets GPIO46 LOW (battery mode only)
            }
        }
        else if (!pressed && prev_pressed)
        {
            // Button released
            uint32_t held_ms = now_ms - press_start_ms;
            ESP_LOGI(kLogTag, "BtnA released after %lu ms", held_ms);

            if (held_ms < kLongPressMs)
            {
                ESP_LOGI(kLogTag, "BtnA click (short press)");
                // Route button click to active screen
                ScreenManager::instance().handle_button_click();
            }
        }

        prev_pressed = pressed;

        // Update active screen
        ScreenManager::instance().tick();
    }

} // namespace app_tasks
