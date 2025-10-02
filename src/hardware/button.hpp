#pragma once

#include <cstdint>
#include <driver/gpio.h>

namespace hardware {

/// Debounced button with short press and long press detection.
/// Uses GPIO polling with configurable debounce time.
class Button {
public:
    /// Callback type for button events
    using Callback = void(*)();

    /// Construct a button on the specified GPIO pin
    /// @param pin GPIO pin number (active low with internal pullup)
    /// @param debounce_ms Debounce time in milliseconds (default 100ms)
    /// @param long_press_ms Long press threshold in milliseconds (default 2000ms)
    explicit Button(gpio_num_t pin, uint32_t debounce_ms = 100, uint32_t long_press_ms = 2000);

    /// Poll the button state (call from main loop)
    void update();

    /// Set callback for short press events (released before long press threshold)
    void on_short_press(Callback cb) { short_press_cb_ = cb; }

    /// Set callback for long press events (held for long_press_ms)
    void on_long_press(Callback cb) { long_press_cb_ = cb; }

    /// Get current debounced state
    bool is_pressed() const { return debounced_state_; }

    /// Get how long the button has been held (0 if not pressed)
    uint32_t held_duration_ms() const;

private:
    gpio_num_t pin_;
    uint32_t debounce_ms_;
    uint32_t long_press_ms_;

    bool debounced_state_ = false;
    bool prev_state_ = false;
    uint32_t last_change_ms_ = 0;
    uint32_t press_start_ms_ = 0;
    bool long_press_triggered_ = false;

    Callback short_press_cb_ = nullptr;
    Callback long_press_cb_ = nullptr;
};

} // namespace hardware
