#pragma once

#include <cstdint>

/// Global game state shared across screens.
/// Implements singleton pattern for easy access.
/// Provides encapsulated access to prevent invalid state modifications.
class GameState {
public:
    /// Get the singleton instance.
    static GameState& instance();

    /// Reset to initial state (useful for restarting game).
    void reset();

    // Configuration getters
    int small_blind() const { return small_blind_; }
    int big_blind() const { return big_blind_; }
    int round_minutes() const { return round_minutes_; }
    float blind_multiplier() const { return blind_multiplier_; }

    // Active game state getters
    int current_round() const { return current_round_; }
    int seconds_remaining() const { return seconds_remaining_; }

    // Configuration setters with validation
    /// Set small blind value (automatically updates big blind to 2x)
    /// @param value Small blind amount (must be positive)
    void set_small_blind(int value);

    /// Set round duration in minutes
    /// @param minutes Duration (must be positive)
    void set_round_minutes(int minutes);

    /// Set blind progression multiplier
    /// @param multiplier Blind increase rate (1.25=RELAXED, 1.5=STANDARD, 2.0=TURBO)
    void set_blind_multiplier(float multiplier);

    // Active game state setters
    /// Set current round number
    /// @param round Round number (must be positive)
    void set_current_round(int round);

    /// Set remaining seconds in current round
    /// @param seconds Remaining time (must be non-negative)
    void set_seconds_remaining(int seconds);

    /// Decrement seconds remaining (used by game timer)
    /// @return New seconds_remaining value
    int decrement_seconds();

    /// Update blind values for next round (maintains small_blind/big_blind invariant)
    /// @param new_small_blind New small blind value
    void update_blinds(int new_small_blind);

private:
    GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // Configuration (set by setup screens)
    int small_blind_ = 25;
    int big_blind_ = 50;
    int round_minutes_ = 15;
    float blind_multiplier_ = 1.5f;  // STANDARD progression (1.25=RELAXED, 1.5=STANDARD, 2.0=TURBO)

    // Active game state
    int current_round_ = 1;
    int seconds_remaining_ = 0;
};
