#pragma once

#include <lvgl.h>

namespace ui::styles
{
    /// Apply dark grey banner styling (used for page titles)
    void apply_title_bg(lv_obj_t *obj);

    /// Apply page title text styling (grey, centered)
    void apply_title_text(lv_obj_t *obj);

    /// Apply big number styling (48pt purple, centered)
    void apply_big_number(lv_obj_t *obj);

    /// Apply bottom button styling (orange background, 240x60)
    void apply_bottom_button(lv_obj_t *obj);

    /// Apply bottom button label styling (black text, centered)
    void apply_bottom_button_label(lv_obj_t *obj);

    /// Apply info button styling (grey circle with "i")
    void apply_info_button(lv_obj_t *obj);

    /// Apply overlay background styling (90% opaque black, no scrolling)
    void apply_overlay_bg(lv_obj_t *obj);
}
