#include "encoder.hpp"
#include <esp_log.h>

namespace hardware {

namespace {
constexpr const char* kLogTag = "encoder";
}

Encoder& Encoder::instance() {
    static Encoder instance;
    return instance;
}

void Encoder::notify_rotation(int delta) {
    if (delta == 0) {
        return;
    }

    ESP_LOGD(kLogTag, "Rotation: %d", delta);

    if (rotation_cb_) {
        rotation_cb_(delta);
    }
}

} // namespace hardware
