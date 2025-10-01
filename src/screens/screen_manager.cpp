#include "screen_manager.hpp"

#include <esp_log.h>

namespace {
constexpr const char* kLogTag = "screen_manager";
}

ScreenManager& ScreenManager::instance() {
    static ScreenManager instance;
    return instance;
}

void ScreenManager::init() {
    ESP_LOGI(kLogTag, "Initialized");
}

void ScreenManager::transition_to(Screen* next_screen) {
    if (next_screen == nullptr) {
        ESP_LOGW(kLogTag, "Attempted to transition to null screen");
        return;
    }

    if (current_ == next_screen) {
        ESP_LOGW(kLogTag, "Already on target screen, ignoring transition");
        return;
    }

    ESP_LOGI(kLogTag, "Transitioning from %p to %p",
             static_cast<void*>(current_),
             static_cast<void*>(next_screen));

    if (current_ != nullptr) {
        current_->on_exit();
    }

    current_ = next_screen;
    current_->on_enter();
}

void ScreenManager::handle_encoder(int diff) {
    if (current_ != nullptr) {
        current_->handle_encoder(diff);
    }
}

void ScreenManager::handle_button_click() {
    if (current_ != nullptr) {
        current_->handle_button_click();
    }
}

void ScreenManager::tick() {
    if (current_ != nullptr) {
        current_->tick();
    }
}
