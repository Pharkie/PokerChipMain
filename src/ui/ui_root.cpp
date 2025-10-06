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
        lv_obj_clear_flag(g_handles.screen, LV_OBJ_FLAG_SCROLLABLE); // Disable swipe/drag scrolling
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
        lv_obj_set_pos(g_handles.big_number, 79, 108);

        // Round title background banner (top area, dark gray - mirrors bottom Menu button)
        // Created FIRST so it's behind the text (z-order)
        g_handles.page_title_bg = lv_obj_create(g_handles.screen);
        lv_obj_clear_flag(g_handles.page_title_bg, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(g_handles.page_title_bg, 280, 50);
        lv_obj_set_pos(g_handles.page_title_bg, -20, -5);
        lv_obj_set_style_bg_color(g_handles.page_title_bg, lv_color_hex(0x333333), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(g_handles.page_title_bg, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(g_handles.page_title_bg, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.page_title_bg, 0, LV_PART_MAIN);

        g_handles.page_title = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.page_title, "");
        apply_label_style(g_handles.page_title, lv_color_hex(0x9A9A9A), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.page_title, LV_ALIGN_TOP_MID, 0, 20);

        // Bottom button bar (shared between config "Confirm" and game "Menu")
        g_handles.bottom_button_bg = lv_button_create(g_handles.screen);
        lv_obj_set_size(g_handles.bottom_button_bg, 240, 60);
        lv_obj_set_pos(g_handles.bottom_button_bg, 0, 200);
        lv_obj_set_style_bg_color(g_handles.bottom_button_bg, lv_color_hex(0xDF7B0F), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(g_handles.bottom_button_bg, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(g_handles.bottom_button_bg, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.bottom_button_bg, 0, LV_PART_MAIN);

        g_handles.confirm_label = lv_label_create(g_handles.bottom_button_bg);
        lv_label_set_text(g_handles.confirm_label, "Confirm");
        lv_obj_set_style_text_color(g_handles.confirm_label, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_align(g_handles.confirm_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(g_handles.confirm_label, LV_ALIGN_CENTER, 0, -12);

        g_handles.menu_label = lv_label_create(g_handles.bottom_button_bg);
        lv_label_set_text(g_handles.menu_label, "Menu");
        lv_obj_set_style_text_color(g_handles.menu_label, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_align(g_handles.menu_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(g_handles.menu_label, LV_ALIGN_CENTER, 0, -12); // Center within button like Menu

        g_handles.small_blind_active = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.small_blind_active, "0");
        apply_label_style(g_handles.small_blind_active, lv_color_hex(0x00FF46), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.small_blind_active, LV_ALIGN_CENTER, -60, -15);

        g_handles.active_small_blind_label = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.active_small_blind_label, "Small blind");
        apply_label_style(g_handles.active_small_blind_label, lv_color_hex(0x00FF46), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.active_small_blind_label, LV_ALIGN_CENTER, -60, -55);

        g_handles.big_blind_active = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.big_blind_active, "0");
        apply_label_style(g_handles.big_blind_active, lv_color_hex(0x00FBFF), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.big_blind_active, LV_ALIGN_CENTER, 60, -15);

        g_handles.active_big_blind_label = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.active_big_blind_label, "Big blind");
        apply_label_style(g_handles.active_big_blind_label, lv_color_hex(0x00FBFF), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.active_big_blind_label, LV_ALIGN_CENTER, 60, -55);

        g_handles.elapsed_mins = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.elapsed_mins, "00");
        apply_label_style(g_handles.elapsed_mins, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.elapsed_mins, LV_ALIGN_CENTER, -40, 35);

        g_handles.elapsed_secs = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.elapsed_secs, "00");
        apply_label_style(g_handles.elapsed_secs, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
        lv_obj_align(g_handles.elapsed_secs, LV_ALIGN_CENTER, 40, 35);

        g_handles.timer_colon = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.timer_colon, ":");
        apply_label_style(g_handles.timer_colon, lv_color_hex(0xFFFFFF), &lv_font_montserrat_48);
        lv_obj_align(g_handles.timer_colon, LV_ALIGN_CENTER, 0, 35);

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

        // Menu overlay (full screen, initially hidden)
        g_handles.menu_overlay = lv_obj_create(g_handles.screen);
        lv_obj_set_size(g_handles.menu_overlay, 240, 240);
        lv_obj_set_pos(g_handles.menu_overlay, 0, 0);
        lv_obj_set_style_bg_color(g_handles.menu_overlay, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(g_handles.menu_overlay, LV_OPA_90, LV_PART_MAIN);
        lv_obj_set_style_border_width(g_handles.menu_overlay, 0, LV_PART_MAIN);
        lv_obj_clear_flag(g_handles.menu_overlay, LV_OBJ_FLAG_SCROLLABLE); // Disable scrolling

        // "Paused MM:SS" label at top
        g_handles.menu_paused_note = lv_label_create(g_handles.menu_overlay);
        lv_label_set_text(g_handles.menu_paused_note, "Paused");
        apply_label_style(g_handles.menu_paused_note, lv_color_hex(0xCCCCCC), LV_FONT_DEFAULT);
        lv_obj_set_style_text_align(g_handles.menu_paused_note, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(g_handles.menu_paused_note, LV_ALIGN_TOP_MID, 0, 20);

        // Menu items (5 buttons, 28px height, 32px spacing = 160px total)
        // Positioned below paused note: y=55, 87, 119, 151, 183 (ends at 211)
        // 280px width intentionally wider than 240px screen to clip at edges (emphasizes circular display)
        g_handles.menu_item_resume = lv_button_create(g_handles.menu_overlay);
        lv_obj_set_size(g_handles.menu_item_resume, 280, 28);
        lv_obj_align(g_handles.menu_item_resume, LV_ALIGN_TOP_MID, 0, 55);
        lv_obj_set_style_bg_color(g_handles.menu_item_resume, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.menu_item_resume, 0, LV_PART_MAIN); // No radius since edges clip
        lv_obj_t *resume_label = lv_label_create(g_handles.menu_item_resume);
        lv_label_set_text(resume_label, "Resume");
        lv_obj_center(resume_label);

        g_handles.menu_item_skip = lv_button_create(g_handles.menu_overlay);
        lv_obj_set_size(g_handles.menu_item_skip, 280, 28);
        lv_obj_align(g_handles.menu_item_skip, LV_ALIGN_TOP_MID, 0, 87);
        lv_obj_set_style_bg_color(g_handles.menu_item_skip, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.menu_item_skip, 0, LV_PART_MAIN);
        lv_obj_t *skip_label = lv_label_create(g_handles.menu_item_skip);
        lv_label_set_text(skip_label, "Skip Round");
        lv_obj_center(skip_label);

        g_handles.menu_item_volume = lv_button_create(g_handles.menu_overlay);
        lv_obj_set_size(g_handles.menu_item_volume, 280, 28);
        lv_obj_align(g_handles.menu_item_volume, LV_ALIGN_TOP_MID, 0, 119);
        lv_obj_set_style_bg_color(g_handles.menu_item_volume, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.menu_item_volume, 0, LV_PART_MAIN);
        lv_obj_t *volume_label = lv_label_create(g_handles.menu_item_volume);
        lv_label_set_text(volume_label, "Volume");
        lv_obj_center(volume_label);

        g_handles.menu_item_reset = lv_button_create(g_handles.menu_overlay);
        lv_obj_set_size(g_handles.menu_item_reset, 280, 28);
        lv_obj_align(g_handles.menu_item_reset, LV_ALIGN_TOP_MID, 0, 151);
        lv_obj_set_style_bg_color(g_handles.menu_item_reset, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.menu_item_reset, 0, LV_PART_MAIN);
        lv_obj_t *reset_label = lv_label_create(g_handles.menu_item_reset);
        lv_label_set_text(reset_label, "New Game");
        lv_obj_center(reset_label);

        g_handles.menu_item_poweroff = lv_button_create(g_handles.menu_overlay);
        lv_obj_set_size(g_handles.menu_item_poweroff, 280, 90);
        lv_obj_align(g_handles.menu_item_poweroff, LV_ALIGN_TOP_MID, 0, 183);
        lv_obj_set_style_bg_color(g_handles.menu_item_poweroff, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.menu_item_poweroff, 0, LV_PART_MAIN);
        lv_obj_t *poweroff_label = lv_label_create(g_handles.menu_item_poweroff);
        lv_label_set_text(poweroff_label, "Power Off");
        lv_obj_set_style_text_align(poweroff_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(poweroff_label, LV_ALIGN_TOP_MID, 0, 4);

        // Info button (right side, positioned inside circular display area)
        g_handles.info_button = lv_button_create(g_handles.screen);
        lv_obj_set_size(g_handles.info_button, 34, 34);
        lv_obj_set_pos(g_handles.info_button, 185, 103); // Right side, vertically centered for circular screen
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
        lv_obj_align(info_title, LV_ALIGN_TOP_MID, 0, 65); // Below Close button

        // Blue chips line
        lv_obj_t *info_blue = lv_label_create(g_handles.info_overlay);
        lv_label_set_text(info_blue, "16 x Blue (25) = 400");
        apply_label_style(info_blue, lv_color_hex(0x4488FF), LV_FONT_DEFAULT);
        lv_obj_set_style_text_align(info_blue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(info_blue, LV_ALIGN_CENTER, 0, -10);

        // White chips line
        lv_obj_t *info_white = lv_label_create(g_handles.info_overlay);
        lv_label_set_text(info_white, "20 x White (50) = 1000");
        apply_label_style(info_white, lv_color_hex(0xFFFFFF), LV_FONT_DEFAULT);
        lv_obj_set_style_text_align(info_white, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(info_white, LV_ALIGN_CENTER, 0, 15);

        // Red chips line
        lv_obj_t *info_red = lv_label_create(g_handles.info_overlay);
        lv_label_set_text(info_red, "6 x Red (100) = 600");
        apply_label_style(info_red, lv_color_hex(0xFF4444), LV_FONT_DEFAULT);
        lv_obj_set_style_text_align(info_red, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(info_red, LV_ALIGN_CENTER, 0, 40);

        // Total stack (after chip breakdown)
        lv_obj_t *info_stack = lv_label_create(g_handles.info_overlay);
        lv_label_set_text(info_stack, "Total stack: 2000");
        apply_label_style(info_stack, lv_color_hex(0xCCCCCC), LV_FONT_DEFAULT);
        lv_obj_set_style_text_align(info_stack, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(info_stack, LV_ALIGN_CENTER, 0, 70);

        // Close button with label (upper-right, positioned inside circular display)
        g_handles.info_close_button = lv_button_create(g_handles.info_overlay);
        lv_obj_set_size(g_handles.info_close_button, 60, 30);
        lv_obj_set_pos(g_handles.info_close_button, 130, 20);
        lv_obj_set_style_bg_color(g_handles.info_close_button, lv_color_hex(0x444444), LV_PART_MAIN);
        lv_obj_set_style_radius(g_handles.info_close_button, 15, LV_PART_MAIN);
        lv_obj_set_style_border_width(g_handles.info_close_button, 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_handles.info_close_button, lv_color_hex(0x888888), LV_PART_MAIN);
        lv_obj_t *close_label = lv_label_create(g_handles.info_close_button);
        lv_label_set_text(close_label, "Close X");
        lv_obj_set_style_text_color(close_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_center(close_label);

        // Initialize widget groups for efficient show/hide management
        g_groups.config_common = {
            &g_handles.page_title,
            &g_handles.big_number,
            &g_handles.bottom_button_bg,
            &g_handles.confirm_label
        };

        g_groups.game_active = {
            &g_handles.page_title,
            &g_handles.page_title_bg,
            &g_handles.small_blind_active,
            &g_handles.big_blind_active,
            &g_handles.active_small_blind_label,
            &g_handles.active_big_blind_label,
            &g_handles.elapsed_mins,
            &g_handles.elapsed_secs,
            &g_handles.timer_colon,
            &g_handles.bottom_button_bg,
            &g_handles.menu_label
        };

        // Menu overlay
        g_groups.menu = {
            &g_handles.menu_overlay,
            &g_handles.menu_item_resume,
            &g_handles.menu_item_reset,
            &g_handles.menu_item_skip,
            &g_handles.menu_item_volume,
            &g_handles.menu_item_poweroff,
            &g_handles.menu_paused_note};

        // Info overlay
        g_groups.info = {
            &g_handles.info_overlay,
            &g_handles.info_button,
            &g_handles.info_close_button};

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

    void show_group(const std::vector<lv_obj_t **> &group)
    {
        for (auto widget_ptr : group)
        {
            if (widget_ptr && *widget_ptr)
            {
                hide(*widget_ptr, false);
            }
        }
    }

    void hide_group(const std::vector<lv_obj_t **> &group)
    {
        for (auto widget_ptr : group)
        {
            if (widget_ptr && *widget_ptr)
            {
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
