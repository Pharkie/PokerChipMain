#include "ui_root.hpp"

#include <lvgl.h>

#include "ui_assets.hpp"

namespace ui
{
namespace
{
Handles g_handles;
WidgetGroups g_groups;

void apply_label_style(lv_obj_t *label, lv_color_t color, const lv_font_t *font)
{
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    if (font != nullptr)
    {
        lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    }
}

void hide(lv_obj_t *obj, bool should_hide)
{
    if (!obj)
    {
        return;
    }
    if (should_hide)
    {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}
} // namespace

void ui_init()
{
    lv_disp_t *disp = lv_disp_get_default();
    g_handles.screen = lv_obj_create(nullptr);
    lv_obj_remove_style_all(g_handles.screen);
    if (disp != nullptr)
    {
        lv_obj_set_size(g_handles.screen, lv_disp_get_hor_res(disp), lv_disp_get_ver_res(disp));
    }
    else
    {
        lv_obj_set_size(g_handles.screen, 240, 240);
    }
    lv_obj_set_style_bg_color(g_handles.screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_handles.screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_screen_load(g_handles.screen);

    // Placeholder for /flash/res/img/riccy.jpeg (UIFlow2Micropython.py:L154)
    g_handles.logo = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.logo, "LOGO");
    lv_obj_set_style_text_color(g_handles.logo, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_pos(g_handles.logo, 10, 10);
    ui::assets::apply_boot_logo(g_handles.logo);

    g_handles.big_number = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.big_number, "0");
    apply_label_style(g_handles.big_number, lv_color_hex(0xFF00DC), &lv_font_montserrat_48);
    lv_obj_set_pos(g_handles.big_number, 79, 100);

    g_handles.page_title = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.page_title, "");
    apply_label_style(g_handles.page_title, lv_color_hex(0x9A9A9A), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.page_title, 20, 62);

    g_handles.pushtext_bg = lv_obj_create(g_handles.screen);
    lv_obj_clear_flag(g_handles.pushtext_bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(g_handles.pushtext_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(g_handles.pushtext_bg, 251, 62);
    lv_obj_set_pos(g_handles.pushtext_bg, -3, 190);
    lv_obj_set_style_bg_color(g_handles.pushtext_bg, lv_color_hex(0xDF7B0F), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_handles.pushtext_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.pushtext_bg, 0, LV_PART_MAIN);

    g_handles.push_text = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.push_text, "Push bezel");
    apply_label_style(g_handles.push_text, lv_color_hex(0x000000), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.push_text, 97, 202);

    g_handles.down_arrow = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.down_arrow, LV_SYMBOL_DOWN);
    apply_label_style(g_handles.down_arrow, lv_color_hex(0x000000), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.down_arrow, 110, 225);

    g_handles.small_blind_active = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.small_blind_active, "0");
    apply_label_style(g_handles.small_blind_active, lv_color_hex(0x00FF46), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.small_blind_active, 88, 58);

    g_handles.elapsed_mins = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.elapsed_mins, "00");
    apply_label_style(g_handles.elapsed_mins, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.elapsed_mins, 94, 191);

    g_handles.big_blind_active = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.big_blind_active, "0");
    apply_label_style(g_handles.big_blind_active, lv_color_hex(0x00FBFF), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.big_blind_active, 71, 121);

    g_handles.elapsed_secs = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.elapsed_secs, "00");
    apply_label_style(g_handles.elapsed_secs, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.elapsed_secs, 137, 202);

    // Timer colon (between MM and SS)
    g_handles.timer_colon = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.timer_colon, ":");
    apply_label_style(g_handles.timer_colon, lv_color_hex(0xFFFFFF), &lv_font_montserrat_48);
    lv_obj_align(g_handles.timer_colon, LV_ALIGN_CENTER, 0, 50);

    g_handles.active_small_blind_label = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.active_small_blind_label, "Small blind");
    apply_label_style(g_handles.active_small_blind_label, lv_color_hex(0x00FF46), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.active_small_blind_label, 5, 82);

    g_handles.active_big_blind_label = lv_label_create(g_handles.screen);
    lv_label_set_text(g_handles.active_big_blind_label, "Big blind");
    apply_label_style(g_handles.active_big_blind_label, lv_color_hex(0x00FBFF), LV_FONT_DEFAULT);
    lv_obj_set_pos(g_handles.active_big_blind_label, 5, 143);

    g_handles.focus_proxy = lv_button_create(g_handles.screen);
    lv_obj_set_size(g_handles.focus_proxy, 1, 1);
    lv_obj_set_pos(g_handles.focus_proxy, 0, 0);
    lv_obj_set_style_bg_opa(g_handles.focus_proxy, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.focus_proxy, 0, LV_PART_MAIN);
#ifdef LV_OBJ_FLAG_FOCUSABLE
    lv_obj_add_flag(g_handles.focus_proxy, LV_OBJ_FLAG_FOCUSABLE);
#endif
    lv_obj_add_flag(g_handles.focus_proxy, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(g_handles.focus_proxy, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);

    // Menu button (bottom center, larger for touch)
    g_handles.menu_button = lv_button_create(g_handles.screen);
    lv_obj_set_size(g_handles.menu_button, 100, 45);
    lv_obj_align(g_handles.menu_button, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_set_style_bg_color(g_handles.menu_button, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.menu_button, 8, LV_PART_MAIN);
    lv_obj_t *menu_label = lv_label_create(g_handles.menu_button);
    lv_label_set_text(menu_label, "MENU");
    lv_obj_center(menu_label);

    // Menu overlay (full screen, initially hidden)
    g_handles.menu_overlay = lv_obj_create(g_handles.screen);
    lv_obj_set_size(g_handles.menu_overlay, 240, 240);
    lv_obj_set_pos(g_handles.menu_overlay, 0, 0);
    lv_obj_set_style_bg_color(g_handles.menu_overlay, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_handles.menu_overlay, LV_OPA_90, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.menu_overlay, 0, LV_PART_MAIN);

    // Menu items (4 buttons with larger height for better touch targets)
    g_handles.menu_item_resume = lv_button_create(g_handles.menu_overlay);
    lv_obj_set_size(g_handles.menu_item_resume, 180, 38);
    lv_obj_align(g_handles.menu_item_resume, LV_ALIGN_CENTER, 0, -75);
    lv_obj_set_style_bg_color(g_handles.menu_item_resume, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.menu_item_resume, 6, LV_PART_MAIN);
    lv_obj_t *resume_label = lv_label_create(g_handles.menu_item_resume);
    lv_label_set_text(resume_label, "Resume");
    lv_obj_center(resume_label);

    g_handles.menu_item_reset = lv_button_create(g_handles.menu_overlay);
    lv_obj_set_size(g_handles.menu_item_reset, 180, 38);
    lv_obj_align(g_handles.menu_item_reset, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_bg_color(g_handles.menu_item_reset, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.menu_item_reset, 6, LV_PART_MAIN);
    lv_obj_t *reset_label = lv_label_create(g_handles.menu_item_reset);
    lv_label_set_text(reset_label, "New Game");
    lv_obj_center(reset_label);

    g_handles.menu_item_skip = lv_button_create(g_handles.menu_overlay);
    lv_obj_set_size(g_handles.menu_item_skip, 180, 38);
    lv_obj_align(g_handles.menu_item_skip, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_bg_color(g_handles.menu_item_skip, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.menu_item_skip, 6, LV_PART_MAIN);
    lv_obj_t *skip_label = lv_label_create(g_handles.menu_item_skip);
    lv_label_set_text(skip_label, "Skip Round");
    lv_obj_center(skip_label);

    g_handles.menu_item_poweroff = lv_button_create(g_handles.menu_overlay);
    lv_obj_set_size(g_handles.menu_item_poweroff, 180, 38);
    lv_obj_align(g_handles.menu_item_poweroff, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_bg_color(g_handles.menu_item_poweroff, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.menu_item_poweroff, 6, LV_PART_MAIN);
    lv_obj_t *poweroff_label = lv_label_create(g_handles.menu_item_poweroff);
    lv_label_set_text(poweroff_label, "Power Off");
    lv_obj_center(poweroff_label);

    // Small "Paused" note below menu items
    g_handles.menu_paused_note = lv_label_create(g_handles.menu_overlay);
    lv_label_set_text(g_handles.menu_paused_note, "Paused");
    apply_label_style(g_handles.menu_paused_note, lv_color_hex(0x888888), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(g_handles.menu_paused_note, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(g_handles.menu_paused_note, LV_ALIGN_BOTTOM_MID, 0, -5);

    // Info button (right side, positioned inside circular display area)
    g_handles.info_button = lv_button_create(g_handles.screen);
    lv_obj_set_size(g_handles.info_button, 34, 34);
    lv_obj_set_pos(g_handles.info_button, 185, 103);  // Right side, vertically centered for circular screen
    lv_obj_set_style_bg_color(g_handles.info_button, lv_color_hex(0x666666), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.info_button, 17, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.info_button, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(g_handles.info_button, lv_color_hex(0x999999), LV_PART_MAIN);
    lv_obj_t *info_label = lv_label_create(g_handles.info_button);
    lv_label_set_text(info_label, "i");
    lv_obj_set_style_text_color(info_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(info_label);

    // Info overlay (full screen, initially hidden)
    g_handles.info_overlay = lv_obj_create(g_handles.screen);
    lv_obj_set_size(g_handles.info_overlay, 240, 240);
    lv_obj_set_pos(g_handles.info_overlay, 0, 0);
    lv_obj_set_style_bg_color(g_handles.info_overlay, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_handles.info_overlay, LV_OPA_90, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.info_overlay, 0, LV_PART_MAIN);
    lv_obj_add_flag(g_handles.info_overlay, LV_OBJ_FLAG_CLICKABLE);

    // Info overlay title (below Close button)
    lv_obj_t *info_title = lv_label_create(g_handles.info_overlay);
    lv_label_set_text(info_title, "CHIP BREAKDOWN");
    apply_label_style(info_title, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(info_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_title, LV_ALIGN_TOP_MID, 0, 65);  // Below Close button

    // Blue chips line
    lv_obj_t *info_blue = lv_label_create(g_handles.info_overlay);
    lv_label_set_text(info_blue, "16 x Blue (25) = 400");
    apply_label_style(info_blue, lv_color_hex(0x4488FF), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(info_blue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_blue, LV_ALIGN_CENTER, 0, -10);  // Moved up

    // White chips line
    lv_obj_t *info_white = lv_label_create(g_handles.info_overlay);
    lv_label_set_text(info_white, "20 x White (50) = 1000");
    apply_label_style(info_white, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(info_white, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_white, LV_ALIGN_CENTER, 0, 15);  // Moved up

    // Red chips line
    lv_obj_t *info_red = lv_label_create(g_handles.info_overlay);
    lv_label_set_text(info_red, "6 x Red (100) = 600");
    apply_label_style(info_red, lv_color_hex(0xFF4444), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(info_red, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_red, LV_ALIGN_CENTER, 0, 40);  // Moved up

    // Total stack (after chip breakdown)
    lv_obj_t *info_stack = lv_label_create(g_handles.info_overlay);
    lv_label_set_text(info_stack, "Total stack: 2000");
    apply_label_style(info_stack, lv_color_hex(0xCCCCCC), LV_FONT_DEFAULT);
    lv_obj_set_style_text_align(info_stack, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_stack, LV_ALIGN_CENTER, 0, 70);  // Below red chips with 5px margin

    // Close button with label (upper-right, positioned inside circular display)
    g_handles.info_close_button = lv_button_create(g_handles.info_overlay);
    lv_obj_set_size(g_handles.info_close_button, 60, 30);  // Wider for "Close X"
    lv_obj_set_pos(g_handles.info_close_button, 130, 20);  // Moved 20px left and 5px up
    lv_obj_set_style_bg_color(g_handles.info_close_button, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_radius(g_handles.info_close_button, 15, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_handles.info_close_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(g_handles.info_close_button, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_t *close_label = lv_label_create(g_handles.info_close_button);
    lv_label_set_text(close_label, "Close X");
    lv_obj_set_style_text_color(close_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(close_label);

    // Initialize widget groups for efficient show/hide management
    // Config screens (small blind, round minutes, progression)
    g_groups.config_common = {
        &g_handles.page_title,
        &g_handles.big_number,
        &g_handles.pushtext_bg,
        &g_handles.push_text,
        &g_handles.down_arrow
    };

    // Active game screen widgets
    g_groups.game_active = {
        &g_handles.page_title,  // Used for round number
        &g_handles.small_blind_active,
        &g_handles.big_blind_active,
        &g_handles.active_small_blind_label,
        &g_handles.active_big_blind_label,
        &g_handles.elapsed_mins,
        &g_handles.elapsed_secs,
        &g_handles.timer_colon,
        &g_handles.menu_button
    };

    // Menu overlay (not auto-hidden, managed by screen)
    g_groups.menu = {
        &g_handles.menu_overlay,
        &g_handles.menu_item_resume,
        &g_handles.menu_item_reset,
        &g_handles.menu_item_skip,
        &g_handles.menu_item_poweroff,
        &g_handles.menu_paused_note
    };

    // Info overlay (not auto-hidden, managed by screen)
    g_groups.info = {
        &g_handles.info_overlay,
        &g_handles.info_button,
        &g_handles.info_close_button
    };

    // Hide all groups initially
    hide_all_groups();
    hide_group(g_groups.menu);
    hide_group(g_groups.info);
    hide(g_handles.logo, true);
}

const Handles &get()
{
    return g_handles;
}

const WidgetGroups &groups()
{
    return g_groups;
}

void show_group(const std::vector<lv_obj_t**> &group)
{
    for (auto widget_ptr : group) {
        if (widget_ptr && *widget_ptr) {
            hide(*widget_ptr, false);
        }
    }
}

void hide_group(const std::vector<lv_obj_t**> &group)
{
    for (auto widget_ptr : group) {
        if (widget_ptr && *widget_ptr) {
            hide(*widget_ptr, true);
        }
    }
}

void hide_all_groups()
{
    hide_group(g_groups.config_common);
    hide_group(g_groups.game_active);
    // Note: menu and info overlays are NOT hidden here - they're managed separately
}

} // namespace ui

