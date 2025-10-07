#pragma once

#include <lvgl.h>

namespace ui {
namespace helpers {

/**
 * Creates a button with focus disabled by default.
 *
 * IMPORTANT: This project uses a custom interaction model where:
 * - Rotary encoder directly modifies values (not input group navigation)
 * - Button A always performs the same action (not "confirm selection")
 * - Touch events handle menu interactions directly
 *
 * LVGL's default behavior adds buttons to input groups when clicked,
 * which causes unwanted blue focus outlines. This helper ensures all
 * buttons have LV_OBJ_FLAG_CLICK_FOCUSABLE removed by default.
 *
 * Usage:
 *   lv_obj_t* btn = ui::helpers::create_button(parent);
 *   // Configure size, position, styles, etc.
 *   lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, userdata);
 */
inline lv_obj_t* create_button(lv_obj_t* parent) {
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    return btn;
}

/**
 * Creates a label with common text settings.
 * No special focus handling needed (labels are not focusable by default).
 */
inline lv_obj_t* create_label(lv_obj_t* parent) {
    return lv_label_create(parent);
}

} // namespace helpers
} // namespace ui
