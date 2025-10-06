#pragma once

#include <lvgl.h>

namespace ui
{
/// Minimal global UI handles - only used for boot splash and encoder input
struct Handles
{
    lv_obj_t *screen = nullptr;      // Main LVGL screen
    lv_obj_t *logo = nullptr;        // Boot splash logo
    lv_obj_t *focus_proxy = nullptr; // Encoder input target
};

/// Initialize UI system (creates screen, logo, and focus proxy)
void ui_init();

/// Get global UI handles (minimal - screens create their own widgets)
const Handles &get();
}

