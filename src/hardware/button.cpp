#include "button.hpp"
#include <esp_timer.h>
#include <esp_log.h>

namespace hardware {

namespace {
constexpr const char* kLogTag = "button";
}

Button::Button(gpio_num_t pin, uint32_t debounce_ms, uint32_t long_press_ms)
    : pin_(pin)
    , debounce_ms_(debounce_ms)
    , long_press_ms_(long_press_ms)
{
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = 1ULL << pin_;
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&cfg);

    ESP_LOGI(kLogTag, "Button initialized on GPIO %d (debounce=%lums, long_press=%lums)",
             pin_, debounce_ms_, long_press_ms_);
}

void Button::update() {
    bool raw_pressed = gpio_get_level(pin_) == 0;  // Active low
    uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);

    // Debounce: only update state if stable for debounce_ms_
    if (raw_pressed != debounced_state_) {
        if (now_ms - last_change_ms_ >= debounce_ms_) {
            debounced_state_ = raw_pressed;
            last_change_ms_ = now_ms;
        }
    } else {
        last_change_ms_ = now_ms;
    }

    bool pressed = debounced_state_;

    // Detect press
    if (pressed && !prev_state_) {
        ESP_LOGI(kLogTag, "Button pressed (GPIO %d)", pin_);
        press_start_ms_ = now_ms;
        long_press_triggered_ = false;
    }
    // Detect long press
    else if (pressed && prev_state_) {
        uint32_t held_ms = now_ms - press_start_ms_;
        if (held_ms >= long_press_ms_ && !long_press_triggered_) {
            ESP_LOGI(kLogTag, "Long press detected (GPIO %d, %lums)", pin_, held_ms);
            long_press_triggered_ = true;
            if (long_press_cb_) {
                long_press_cb_();
            }
        }
    }
    // Detect release
    else if (!pressed && prev_state_) {
        uint32_t held_ms = now_ms - press_start_ms_;
        ESP_LOGI(kLogTag, "Button released (GPIO %d, %lums)", pin_, held_ms);

        // Only trigger short press if long press wasn't triggered
        if (held_ms < long_press_ms_ && !long_press_triggered_) {
            ESP_LOGI(kLogTag, "Short press (GPIO %d)", pin_);
            if (short_press_cb_) {
                short_press_cb_();
            }
        }
    }

    prev_state_ = pressed;
}

uint32_t Button::held_duration_ms() const {
    if (!debounced_state_) {
        return 0;
    }
    uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
    return now_ms - press_start_ms_;
}

} // namespace hardware
