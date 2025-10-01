#include "small_blind_screen.hpp"

#include <M5Unified.hpp>
#include <algorithm>
#include <esp_log.h>
#include <lvgl.h>

#include "input/encoder_input.hpp"
#include "ui/ui_root.hpp"

namespace screens::small_blind
{

void handle_encoder_delta(int step);

namespace
{
constexpr const char *kLogTag = "small_blind";

constexpr int kStep = 25;
constexpr int kMin = 25;
constexpr int kMax = 200;
constexpr float kToneUp = 3520.0f;      // A7
constexpr float kToneDown = 2793.0f;    // F7
constexpr float kToneBoundary = 1661.0f; // G#6
constexpr uint32_t kToneDuration = 120;  // milliseconds

lv_obj_t *s_controller = nullptr;
bool s_suppress_controller_event = false;

struct State
{
    int value = kMin;
} g_state;

int step_count()
{
    return (kMax - kMin) / kStep;
}

int value_to_index(int value)
{
    return (value - kMin) / kStep;
}

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

void sync_controller()
{
    if (!s_controller)
    {
        return;
    }
    s_suppress_controller_event = true;
    lv_arc_set_value(s_controller, value_to_index(g_state.value));
    s_suppress_controller_event = false;
}

void controller_event_cb(lv_event_t *event)
{
    if (s_suppress_controller_event)
    {
        return;
    }
    if (lv_event_get_code(event) != LV_EVENT_VALUE_CHANGED)
    {
        return;
    }

    lv_obj_t *arc = static_cast<lv_obj_t *>(lv_event_get_target(event));
    int index = lv_arc_get_value(arc);
    int target = kMin + index * kStep;
    int diff = (target - g_state.value) / kStep;
    if (diff != 0)
    {
        handle_encoder_delta(diff);
    }
}

void ensure_controller()
{
    const int max_index = step_count();

    if (!s_controller)
    {
        s_controller = lv_arc_create(lv_screen_active());
        lv_obj_set_size(s_controller, 120, 120);
        lv_obj_set_pos(s_controller, -200, -200); // keep off-screen but available
        lv_arc_set_bg_angles(s_controller, 0, 360);
        lv_arc_set_rotation(s_controller, 0);
        lv_arc_set_range(s_controller, 0, max_index);
        lv_arc_set_value(s_controller, value_to_index(g_state.value));

        // Make the controller invisible.
        lv_obj_set_style_bg_opa(s_controller, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_arc_width(s_controller, 0, LV_PART_MAIN);
        lv_obj_set_style_arc_width(s_controller, 0, LV_PART_INDICATOR);
        lv_obj_set_style_arc_opa(s_controller, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(s_controller, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_obj_set_style_border_width(s_controller, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(s_controller, 0, LV_PART_MAIN);

        lv_obj_add_event_cb(s_controller, controller_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);
    }

    lv_arc_set_range(s_controller, 0, max_index);
    sync_controller();

    if (lv_group_t *group = encoder_input::group())
    {
        lv_group_t *current_group = lv_obj_get_group(s_controller);
        if (current_group != group)
        {
            if (current_group)
            {
                lv_group_remove_obj(s_controller);
            }
            lv_group_add_obj(group, s_controller);
        }
        lv_group_focus_obj(s_controller);
        lv_group_set_editing(group, true);
    }
}

} // namespace

void show()
{
    const auto &handles = ui::get();
    g_state.value = kMin;

    ESP_LOGI(kLogTag, "show -> %d", g_state.value);

    set_visible(handles.logo, false);

    lv_label_set_text(handles.page_title, "Starting small blinds");
    lv_obj_set_style_text_align(handles.page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(handles.page_title, LV_ALIGN_TOP_MID, 0, 40);
    set_visible(handles.page_title, true);

    update_display();
    lv_obj_set_style_text_align(handles.big_number, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_transform_zoom(handles.big_number, 1024, LV_PART_MAIN);
    lv_obj_align(handles.big_number, LV_ALIGN_CENTER, 0, -10);
    set_visible(handles.big_number, true);

    lv_obj_set_width(handles.pushtext_bg, 240);
    lv_obj_set_height(handles.pushtext_bg, 60);
    lv_obj_align(handles.pushtext_bg, LV_ALIGN_BOTTOM_MID, 0, 0);
    set_visible(handles.pushtext_bg, true);

    lv_obj_set_style_text_align(handles.push_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(handles.push_text, LV_ALIGN_BOTTOM_MID, 0, -24);
    set_visible(handles.push_text, true);

    lv_obj_align(handles.down_arrow, LV_ALIGN_BOTTOM_MID, 0, -4);
    set_visible(handles.down_arrow, true);

    set_visible(handles.small_blind_active, false);
    set_visible(handles.big_blind_active, false);
    set_visible(handles.active_small_blind_label, false);
    set_visible(handles.active_big_blind_label, false);
    set_visible(handles.elapsed_mins, false);
    set_visible(handles.elapsed_secs, false);
    set_visible(handles.mins_label, false);
    set_visible(handles.secs_label, false);

    ensure_controller();
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
        sync_controller();
    }
    else if (boundary)
    {
        ESP_LOGI(kLogTag, "boundary tone at %d", next);
        play_tone(kToneBoundary);
        sync_controller();
    }
}

bool handle_raw_encoder_diff(int diff)
{
    if (diff > 0 && g_state.value >= kMax)
    {
        ESP_LOGI(kLogTag, "raw diff=%d at upper bound", diff);
        play_tone(kToneBoundary);
        return true;
    }
    if (diff < 0 && g_state.value <= kMin)
    {
        ESP_LOGI(kLogTag, "raw diff=%d at lower bound", diff);
        play_tone(kToneBoundary);
        return true;
    }
    return false;
}

} // namespace screens::small_blind

