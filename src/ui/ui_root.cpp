#include "ui_root.hpp"

#include <lvgl.h>

#include "ui_assets.hpp"

namespace ui
{
    namespace
    {
        Handles g_handles;
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

        // Boot splash logo
        g_handles.logo = lv_label_create(g_handles.screen);
        lv_label_set_text(g_handles.logo, "LOGO");
        lv_obj_set_style_text_color(g_handles.logo, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_pos(g_handles.logo, 10, 10);
        ui::assets::apply_boot_logo(g_handles.logo);
        lv_obj_add_flag(g_handles.logo, LV_OBJ_FLAG_HIDDEN); // Hidden by default

        // Focus proxy for encoder input
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
    }

    const Handles &get()
    {
        return g_handles;
    }

} // namespace ui
