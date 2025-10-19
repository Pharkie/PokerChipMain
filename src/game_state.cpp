#include "game_state.hpp"
#include <M5Unified.hpp>
#include <esp_log.h>

namespace {
constexpr const char* kLogTag = "game_state";
}

GameState& GameState::instance() {
    static GameState instance;
    return instance;
}

void GameState::reset() {
    small_blind_ = 25;
    big_blind_ = 50;
    round_minutes_ = 15;
    blind_multiplier_ = 1.5f;
    current_round_ = 1;
    seconds_remaining_ = 0;
    total_game_seconds_ = 0;
    total_paused_seconds_ = 0;
    pause_start_ms_ = 0;
    max_round_reached_ = 1;
    ESP_LOGI(kLogTag, "State reset to defaults");
}

void GameState::set_small_blind(int value) {
    if (value <= 0) {
        ESP_LOGW(kLogTag, "Invalid small blind: %d (must be positive)", value);
        return;
    }
    small_blind_ = value;
    big_blind_ = value * 2;  // Maintain invariant
    ESP_LOGI(kLogTag, "Blinds set: SB=%d, BB=%d", small_blind_, big_blind_);
}

void GameState::set_round_minutes(int minutes) {
    if (minutes <= 0) {
        ESP_LOGW(kLogTag, "Invalid round minutes: %d (must be positive)", minutes);
        return;
    }
    round_minutes_ = minutes;
    ESP_LOGI(kLogTag, "Round duration set: %d minutes", round_minutes_);
}

void GameState::set_blind_multiplier(float multiplier) {
    if (multiplier <= 1.0f) {
        ESP_LOGW(kLogTag, "Invalid blind multiplier: %.2f (must be > 1.0)", multiplier);
        return;
    }
    blind_multiplier_ = multiplier;
    ESP_LOGI(kLogTag, "Blind multiplier set: %.2fx", blind_multiplier_);
}

void GameState::set_current_round(int round) {
    if (round <= 0) {
        ESP_LOGW(kLogTag, "Invalid round number: %d (must be positive)", round);
        return;
    }
    current_round_ = round;
}

void GameState::set_seconds_remaining(int seconds) {
    if (seconds < 0) {
        ESP_LOGW(kLogTag, "Invalid seconds: %d (must be non-negative)", seconds);
        return;
    }
    seconds_remaining_ = seconds;
}

int GameState::decrement_seconds() {
    if (seconds_remaining_ > 0) {
        seconds_remaining_--;
    }
    return seconds_remaining_;
}

void GameState::update_blinds(int new_small_blind) {
    if (new_small_blind <= 0) {
        ESP_LOGW(kLogTag, "Invalid small blind update: %d", new_small_blind);
        return;
    }
    small_blind_ = new_small_blind;
    big_blind_ = new_small_blind * 2;  // Maintain invariant
    ESP_LOGI(kLogTag, "Blinds updated: SB=%d, BB=%d", small_blind_, big_blind_);
}

void GameState::start_game_timer() {
    total_game_seconds_ = 0;
    total_paused_seconds_ = 0;
    pause_start_ms_ = 0;
    max_round_reached_ = 1;
    ESP_LOGI(kLogTag, "Game timer started");
}

void GameState::tick_game_timer() {
    total_game_seconds_++;
}

void GameState::pause_game_timer() {
    if (pause_start_ms_ == 0) {
        pause_start_ms_ = M5.millis();
        ESP_LOGI(kLogTag, "Game timer paused at %lus game time", (unsigned long)total_game_seconds_);
    }
}

void GameState::resume_game_timer() {
    if (pause_start_ms_ != 0) {
        uint32_t pause_duration_ms = M5.millis() - pause_start_ms_;
        uint32_t pause_duration_secs = pause_duration_ms / 1000;
        total_paused_seconds_ += pause_duration_secs;
        pause_start_ms_ = 0;
        ESP_LOGI(kLogTag, "Game timer resumed (paused for %lus, total paused: %lus)",
                 (unsigned long)pause_duration_secs, (unsigned long)total_paused_seconds_);
    }
}

void GameState::record_max_round(int round) {
    if (round > max_round_reached_) {
        max_round_reached_ = round;
    }
}
