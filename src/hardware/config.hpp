#pragma once

#include <driver/gpio.h>
#include <cstdint>

namespace hardware {
namespace config {

/// GPIO Pin Assignments for M5Stack Dial
namespace pins {
    /// Button A physical button on device (active low with internal pullup)
    constexpr gpio_num_t BUTTON_A = GPIO_NUM_42;

    /// Rotary encoder pins (handled by PCNT peripheral - see encoder.cpp)
    /// Note: Actual pin config is in encoder_input.cpp, these are for reference
    // constexpr gpio_num_t ENCODER_A = GPIO_NUM_XX;  // TODO: Document encoder pins
    // constexpr gpio_num_t ENCODER_B = GPIO_NUM_XX;
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
