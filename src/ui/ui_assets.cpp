#include "ui_assets.hpp"

#include <lvgl.h>

namespace ui::assets
{
void init()
{
    // TODO: Mount LittleFS and load runtime assets when available.
}

bool apply_boot_logo(lv_obj_t *obj)
{
    LV_UNUSED(obj);
    // TODO: Load /flash/res/img/riccy.jpeg from the asset pack if provided.
    return false;
}
}

