#include "encoder_input.hpp"

namespace encoder_input
{
namespace
{
lv_group_t *s_group = nullptr;
}

void init(lv_obj_t *focus_target)
{
    if (focus_target == nullptr)
    {
        return;
    }

    if (s_group == nullptr)
    {
        s_group = lv_group_create();
        lv_group_set_wrap(s_group, false);
    }

    lv_group_remove_all_objs(s_group);
    lv_group_add_obj(s_group, focus_target);

    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev != nullptr; indev = lv_indev_get_next(indev))
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER)
        {
            lv_indev_set_group(indev, s_group);
        }
    }

    lv_group_focus_obj(focus_target);
}

lv_group_t *group()
{
    return s_group;
}
}

