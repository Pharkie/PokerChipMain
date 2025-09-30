#pragma once

#include <lvgl.h>

namespace ui
{
struct Handles
{
    lv_obj_t *screen = nullptr;
    lv_obj_t *logo = nullptr;
    lv_obj_t *big_number = nullptr;
    lv_obj_t *page_title = nullptr;
    lv_obj_t *pushtext_bg = nullptr;
    lv_obj_t *push_text = nullptr;
    lv_obj_t *down_arrow = nullptr;
    lv_obj_t *small_blind_active = nullptr;
    lv_obj_t *elapsed_mins = nullptr;
    lv_obj_t *big_blind_active = nullptr;
    lv_obj_t *mins_label = nullptr;
    lv_obj_t *elapsed_secs = nullptr;
    lv_obj_t *secs_label = nullptr;
    lv_obj_t *active_small_blind_label = nullptr;
    lv_obj_t *active_big_blind_label = nullptr;
    lv_obj_t *focus_proxy = nullptr;
};

void ui_init();
const Handles &get();
}

