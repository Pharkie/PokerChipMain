#include "screen.hpp"

#include <M5Unified.hpp>
#include "ui/ui_root.hpp"

const ui::Handles& Screen::ui() const {
    return ui::get();
}

void Screen::set_visible(lv_obj_t* obj, bool visible) {
    if (!obj) {
        return;
    }
    if (visible) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void Screen::play_tone(float freq_hz, uint32_t duration_ms) {
    if (freq_hz > 0.0f) {
        M5.Speaker.tone(freq_hz, duration_ms);
    }
}

bool Screen::is_modal_blocking() const {
    // Check if info overlay is visible (single source of truth)
    return ui().info_overlay &&
           !lv_obj_has_flag(ui().info_overlay, LV_OBJ_FLAG_HIDDEN);
}
