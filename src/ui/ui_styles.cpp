#include "ui_styles.hpp"

namespace ui::styles
{
    void apply_title_bg(lv_obj_t *obj)
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(obj, 280, 58);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    }

    void apply_title_text(lv_obj_t *obj)
    {
        lv_obj_set_style_text_color(obj, lv_color_hex(0x9A9A9A), LV_PART_MAIN);
        lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }

    void apply_big_number(lv_obj_t *obj)
    {
        lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN);
        lv_obj_set_style_text_color(obj, lv_color_hex(0xFF00DC), LV_PART_MAIN);
        lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }

    void apply_bottom_button(lv_obj_t *obj)
    {
        lv_obj_set_size(obj, 240, 60);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xDF7B0F), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    }

    void apply_bottom_button_label(lv_obj_t *obj)
    {
        lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }

    void apply_info_button(lv_obj_t *obj)
    {
        lv_obj_set_size(obj, 34, 34);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), LV_PART_MAIN);
        lv_obj_set_style_radius(obj, 17, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN);
        lv_obj_set_style_border_color(obj, lv_color_hex(0x999999), LV_PART_MAIN);
    }

    void apply_overlay_bg(lv_obj_t *obj)
    {
        lv_obj_set_size(obj, 240, 240);
        lv_obj_set_style_bg_color(obj, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj, LV_OPA_90, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    }
}
