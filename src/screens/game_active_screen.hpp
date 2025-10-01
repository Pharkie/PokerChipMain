#pragma once

#include "screen.hpp"
#include <cstdint>

/// Active game screen showing timer countdown and current blinds.
/// Displays round number, small/big blind values, and countdown timer.
/// Auto-advances rounds when timer expires, doubling blinds each time.
class GameActiveScreen : public Screen {
public:
    /// Get the singleton instance.
    static GameActiveScreen& instance();

    // Screen interface implementation
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;  // No-op during game
    void handle_button_click() override;     // Power off device
    void tick() override;                    // 1Hz countdown

private:
    GameActiveScreen() = default;

    uint32_t last_tick_ms_ = 0;

    static constexpr uint32_t kTickIntervalMs = 1000;  // 1 second
    static constexpr int kMaxBlind = 9999;             // Failsafe

    void update_timer_display();
    void update_blind_display();
    void update_round_title();
    void advance_round();
    void play_round_transition_tones();
};
