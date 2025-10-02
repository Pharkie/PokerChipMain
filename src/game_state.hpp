#pragma once

#include <cstdint>

/// Global game state shared across screens.
/// Implements singleton pattern for easy access.
struct GameState {
    // Configuration (set by setup screens)
    int small_blind = 25;
    int big_blind = 50;
    int round_minutes = 15;
    float blind_multiplier = 1.5f;  // STANDARD progression (1.25=RELAXED, 1.5=STANDARD, 2.0=TURBO)

    // Active game state
    int current_round = 1;
    int seconds_remaining = 0;

    /// Get the singleton instance.
    static GameState& instance();

    /// Reset to initial state (useful for restarting game).
    void reset();

private:
    GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
};
