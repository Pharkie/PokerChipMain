#include "small_blind_screen.hpp"

#include <M5Unified.hpp>
#include <algorithm>
#include <esp_log.h>
#include <lvgl.h>

#include "ui/ui_root.hpp"

namespace screens::small_blind
{
namespace
{
constexpr const char *kLogTag = "small_blind";

constexpr int kStep = 25;
constexpr int kMin = 25;
constexpr int kMax = 200;
constexpr float kToneUp = 3520.0f;      // A7
constexpr float kToneDown = 2793.0f;    // F7
constexpr float kToneBoundary = 1661.0f; // G#6
constexpr uint32_t kToneDuration = 120; // milliseconds

struct State
{
    int value = kMin;
} g_state;

void set_visible(lv_obj_t *obj, bool visible)
{
    if (!obj)
    {
        return;
    }
    if (visible)
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void update_display()
{
    const auto &handles = ui::get();
    lv_label_set_text_fmt(handles.big_number, "%d", g_state.value);
}

void play_tone(float freq)
{
    if (freq <= 0.0f)
    {
        return;
    }
    M5.Speaker.tone(freq, kToneDuration);
}

} // namespace

void show()
{
    const auto &handles = ui::get();
    g_state.value = kMin;

    ESP_LOGI(kLogTag, "show -> %d", g_state.value);

    set_visible(handles.logo, false);

    lv_label_set_text(handles.page_title, "Starting small blinds");
    set_visible(handles.page_title, true);

    update_display();
    set_visible(handles.big_number, true);

    set_visible(handles.pushtext_bg, true);
    set_visible(handles.push_text, true);
    set_visible(handles.down_arrow, true);

    set_visible(handles.small_blind_active, false);
    set_visible(handles.big_blind_active, false);
    set_visible(handles.active_small_blind_label, false);
    set_visible(handles.active_big_blind_label, false);
    set_visible(handles.elapsed_mins, false);
    set_visible(handles.elapsed_secs, false);
    set_visible(handles.mins_label, false);
    set_visible(handles.secs_label, false);
}

void handle_encoder_delta(int step)
{
    if (step == 0)
    {
        return;
    }

    ESP_LOGI(kLogTag, "encoder delta=%d", step);

    int next = g_state.value + step * kStep;
    bool boundary = false;

    if (next < kMin)
    {
        next = kMin;
        boundary = true;
    }
    if (next > kMax)
    {
        next = kMax;
        boundary = true;
    }

    if (next != g_state.value)
    {
        g_state.value = next;
        update_display();
        play_tone(step > 0 ? kToneUp : kToneDown);
        ESP_LOGI(kLogTag, "small blind -> %d", g_state.value);
    }
    else if (boundary)
    {
        ESP_LOGI(kLogTag, "boundary tone at %d", next);
        play_tone(kToneBoundary);
    }
}

} // namespace screens::small_blind

