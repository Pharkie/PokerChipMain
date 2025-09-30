#pragma once

#include <lvgl.h>

namespace ui::assets
{
struct ImageAsset
{
    const char *path;    // Source path in LittleFS or other FS
    const lv_img_dsc_t *descriptor; // Optional compiled-in descriptor
};

// Attempt to apply the boot logo to the provided LVGL image/label object.
// Returns true if an image was assigned.
bool apply_boot_logo(lv_obj_t *obj);

// Placeholder for future asset packing hook. Invoked during setup to
// initialise any runtime asset sources (LittleFS, etc.).
void init();
}

