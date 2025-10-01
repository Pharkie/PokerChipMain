#include "app_tasks.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include <lvgl.h>
#include <driver/gpio.h>

#include "input/encoder_input.hpp"
#include "tasks/small_blind_screen.hpp"
#include "ui/ui_assets.hpp"
#include "ui/ui_root.hpp"

namespace app_tasks
{
namespace
{
constexpr const char *kLogTag = "app_tasks";

constexpr float kBootTone1Freq = 3135.0f; // G7
constexpr uint32_t kBootTone1Duration = 100;
constexpr uint32_t kBootTone1Delay = 80;
constexpr float kBootTone2Freq = 4186.0f; // C8
constexpr uint32_t kBootTone2Duration = 120;

constexpr gpio_num_t kBtnAPin = GPIO_NUM_42;

void encoder_key_event_cb(lv_event_t *event)
{
    const uint32_t key = lv_event_get_key(event);
    ESP_LOGI(kLogTag, "encoder key event: %u", static_cast<unsigned>(key));

    switch (key)
    {
    case LV_KEY_RIGHT:
    case LV_KEY_NEXT:
        screens::small_blind::handle_encoder_delta(1);
        break;
    case LV_KEY_LEFT:
    case LV_KEY_PREV:
        screens::small_blind::handle_encoder_delta(-1);
        break;
    default:
        break;
    }
}

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

    M5.Speaker.tone(kBootTone1Freq, kBootTone1Duration);
    M5.delay(kBootTone1Duration + kBootTone1Delay);
    M5.Speaker.tone(kBootTone2Freq, kBootTone2Duration);

    if (handles.focus_proxy)
    {
        lv_obj_add_event_cb(handles.focus_proxy, encoder_key_event_cb, LV_EVENT_KEY, nullptr);
    }

    if (encoder_input::group() && handles.focus_proxy)
    {
        lv_group_focus_obj(handles.focus_proxy);
    }

    screens::small_blind::show();
}

void tick()
{
    static bool prev_pressed = false;
    bool pressed = gpio_get_level(kBtnAPin) == 0;

    if (pressed && !prev_pressed)
    {
        ESP_LOGI(kLogTag, "BtnA pressed");
    }
    else if (!pressed && prev_pressed)
    {
        ESP_LOGI(kLogTag, "BtnA released");
        ESP_LOGI(kLogTag, "BtnA click");
    }

    prev_pressed = pressed;
}

} // namespace app_tasks

