#pragma once

#include <lvgl.h>
#include <vector>

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
    lv_obj_t *timer_colon = nullptr;
    lv_obj_t *secs_label = nullptr;
    lv_obj_t *active_small_blind_label = nullptr;
    lv_obj_t *active_big_blind_label = nullptr;
    lv_obj_t *focus_proxy = nullptr;

    // Menu system
    lv_obj_t *menu_button = nullptr;
    lv_obj_t *menu_overlay = nullptr;
    lv_obj_t *menu_item_resume = nullptr;
    lv_obj_t *menu_item_reset = nullptr;
    lv_obj_t *menu_item_skip = nullptr;
    lv_obj_t *menu_item_poweroff = nullptr;
    lv_obj_t *menu_paused_note = nullptr;

    // Info system
    lv_obj_t *info_button = nullptr;
    lv_obj_t *info_overlay = nullptr;
    lv_obj_t *info_close_button = nullptr;
};

/// Widget groups for efficient show/hide management
/// Widgets are organized by which screens use them
struct WidgetGroups
{
    /// Configuration screens common widgets (small blind, round minutes, progression)
    std::vector<lv_obj_t**> config_common;

    /// Active game screen widgets
    std::vector<lv_obj_t**> game_active;

    /// Menu overlay widgets (not auto-managed - use show_menu/hide_menu)
    std::vector<lv_obj_t**> menu;

    /// Info overlay widgets (not auto-managed - use show_info/hide_info)
    std::vector<lv_obj_t**> info;
};

void ui_init();
const Handles &get();
const WidgetGroups &groups();

/// Show all widgets in a group
void show_group(const std::vector<lv_obj_t**> &group);

/// Hide all widgets in a group
void hide_group(const std::vector<lv_obj_t**> &group);

/// Hide all managed widget groups (config_common + game_active)
/// Does NOT hide menu/info overlays (those are managed separately)
void hide_all_groups();
}

