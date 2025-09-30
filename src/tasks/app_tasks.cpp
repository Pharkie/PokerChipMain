#include "app_tasks.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include <lvgl.h>

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
constexpr uint32_t kBootTone1Duration = 200;
constexpr uint32_t kBootTone1Delay = 150;
constexpr float kBootTone2Freq = 4186.0f; // C8
constexpr uint32_t kBootTone2Duration = 200;

lv_timer_t *g_second_tone_timer = nullptr;

void second_boot_tone_cb(lv_timer_t *timer)
{
    M5.Speaker.tone(kBootTone2Freq, kBootTone2Duration);
    if (g_second_tone_timer == timer)
    {
        g_second_tone_timer = nullptr;
    }
    lv_timer_del(timer);
}

void schedule_second_tone()
{
    if (g_second_tone_timer)
    {
        lv_timer_del(g_second_tone_timer);
        g_second_tone_timer = nullptr;
    }

    g_second_tone_timer = lv_timer_create(second_boot_tone_cb, kBootTone1Duration + kBootTone1Delay, nullptr);
    if (!g_second_tone_timer)
    {
        ESP_LOGW(kLogTag, "Failed to schedule second boot tone");
    }
}

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

    const auto &handles = ui::get();
    lv_obj_clear_flag(handles.logo, LV_OBJ_FLAG_HIDDEN);

    M5.Speaker.tone(kBootTone1Freq, kBootTone1Duration);
    schedule_second_tone();

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
    // No runtime logic yet – focus on the small blind screen interactions only.
}

} // namespace app_tasks

