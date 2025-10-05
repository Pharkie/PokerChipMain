#pragma once

#include <driver/gpio.h>
#include <cstdint>

namespace hardware {
namespace config {

/// GPIO Pin Assignments for M5Stack Dial
namespace pins {
    /// Button A physical button on device (active low with internal pullup)
    constexpr gpio_num_t BUTTON_A = GPIO_NUM_42;

    // Note: Rotary encoder (GPIO 40/41) is configured in m5dial_lvgl component
}

/// Button timing configuration
namespace button {
    /// Debounce time in milliseconds (filters mechanical bounce)
    constexpr uint32_t DEBOUNCE_MS = 100;

    /// Long press threshold in milliseconds (hold duration to trigger long press)
    constexpr uint32_t LONG_PRESS_MS = 2000;
}

/// Audio feedback configuration
namespace audio {
    /// Default tone duration for UI feedback sounds
    constexpr uint32_t DEFAULT_TONE_DURATION_MS = 120;
}

} // namespace config
} // namespace hardware
