#include "encoder_input.hpp"

#include <esp_log.h>
#include "tasks/small_blind_screen.hpp"

namespace encoder_input
{
namespace
{
constexpr const char *kLogTag = "encoder_input";

lv_group_t *s_group = nullptr;
}

void init(lv_obj_t *focus_target)
{
    if (focus_target == nullptr)
    {
        ESP_LOGW(kLogTag, "Focus target is null; encoder not initialised");
        return;
    }

    if (s_group == nullptr)
    {
        s_group = lv_group_create();
        lv_group_set_wrap(s_group, false);
        lv_group_set_default(s_group);
        ESP_LOGI(kLogTag, "Created encoder group %p", static_cast<void *>(s_group));
    }

    lv_group_remove_all_objs(s_group);
    lv_group_add_obj(s_group, focus_target);
    ESP_LOGI(kLogTag, "Added focus target %p", static_cast<void *>(focus_target));

    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev != nullptr; indev = lv_indev_get_next(indev))
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER)
        {
            lv_indev_set_group(indev, s_group);
            ESP_LOGI(kLogTag, "Bound LVGL encoder indev %p", static_cast<void *>(indev));
        }
    }

    lv_group_focus_obj(focus_target);
    lv_group_set_editing(s_group, true);
}

lv_group_t *group()
{
    return s_group;
}
}

extern "C" void encoder_notify_diff(int diff)
{
    if (diff == 0)
    {
        return;
    }

    if (screens::small_blind::handle_raw_encoder_diff(diff))
    {
        return;
    }

    lv_group_t *grp = encoder_input::group();
    if (grp == nullptr)
    {
        return;
    }

    ESP_LOGI("encoder_input", "forward diff=%d", diff);
    lv_group_set_editing(grp, true);
    lv_group_send_data(grp, diff);
}
